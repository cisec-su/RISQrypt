#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARAMS_SOURCE="$SCRIPT_DIR/../../src/rubato/params.h"
BOOTLOADER="$SCRIPT_DIR/../../../sdk/toolchain/bootloader.py"

# Detect project name from makefile
PROJ_NAME="$(grep '^PROJ = ' "$SCRIPT_DIR/makefile" | awk '{print $NF}')"
if [[ -z "$PROJ_NAME" ]]; then
  PROJ_NAME="rubato_test"  # fallback
fi

# Override with: PORT=/dev/ttyUSB1 ./run_all_configs.sh
PORT="${PORT:-/dev/ttyUSB1}"

PARAMS=(
  PARAM_80_S
  PARAM_80_L
  PARAM_128_S
  PARAM_128_L
)
XOFS=(
  XOF_SHAKE128
  XOF_SHAKE256
)

LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SUMMARY_FILE="$LOG_DIR/run_all_configs.summary"
RAW_LOG="$LOG_DIR/run_all_configs.log"
PARAMS_BACKUP="$SCRIPT_DIR/../.params.h.bak"

# Cleanup legacy backup files created by old script versions.
rm -f "$SCRIPT_DIR"/params.h.bak.* "$PARAMS_BACKUP"
# Backup original params.h from source if it exists; if missing, create a minimal one
CREATED_PARAMS=false
if [[ -f "$PARAMS_SOURCE" ]]; then
  cp "$PARAMS_SOURCE" "$PARAMS_BACKUP"
else
  cat > "$PARAMS_SOURCE" <<'H'
#ifndef PARAMS_H
#define PARAMS_H
#define PARAM_80_S
#define XOF_SHAKE128
#endif
H
  CREATED_PARAMS=true
fi

restore_params() {
  if [[ -f "$PARAMS_BACKUP" ]]; then
    cp "$PARAMS_BACKUP" "$PARAMS_SOURCE"
    rm -f "$PARAMS_BACKUP"
  elif [[ "$CREATED_PARAMS" == "true" ]]; then
    rm -f "$PARAMS_SOURCE"
  fi
}
trap restore_params EXIT

set_config() {
  local param="$1"
  local xof="$2"

  python3 - "$PARAMS_SOURCE" "$param" "$xof" <<'PY'
import pathlib
import re
import sys

path = pathlib.Path(sys.argv[1])
param = sys.argv[2]
xof = sys.argv[3]
text = path.read_text()

text = re.sub(r'^#define\s+PARAM_(80|128)_(S|L)\s*$', f'#define {param}', text, flags=re.M)
text = re.sub(r'^#define\s+XOF_SHAKE(128|256)\s*$', f'#define {xof}', text, flags=re.M)

path.write_text(text)
PY
}

run_one() {
  local param="$1"
  local xof="$2"
  local cycle="NA"
  local masked_cycle="NA"
  local result="UNKNOWN"

  echo "[$(date '+%H:%M:%S')] Running: $param, $xof" >>"$RAW_LOG"

  set_config "$param" "$xof"

  # Rebuild with current configuration
  if ! make -C "$SCRIPT_DIR" >>"$RAW_LOG" 2>&1; then
    result="BUILD_FAIL"
    printf "%-15s %-15s %-12s %-20s %-20s\n" "$param" "$xof" "$result" "cycle=$cycle" "masked_cycle=$masked_cycle" | tee -a "$SUMMARY_FILE"
    return
  fi

  # Flash and run tests with timeout
  local run_output
  local timeout_secs=120
  if ! run_output="$(timeout $timeout_secs bash -c "cd \"$SCRIPT_DIR\" && python3 \"$BOOTLOADER\" -f \"$PROJ_NAME\" -p \"$PORT\" 2>&1" | tr -d '\000')"; then
    local exit_code=$?
    if [[ $exit_code -eq 124 ]]; then
      echo "TIMEOUT: bootloader took longer than ${timeout_secs}s" >>"$RAW_LOG"
      result="TIMEOUT"
    else
      echo "$run_output" >>"$RAW_LOG"
      result="RUN_FAIL"
    fi
    printf "%-15s %-15s %-12s %-20s %-20s\n" "$param" "$xof" "$result" "cycle=$cycle" "masked_cycle=$masked_cycle" | tee -a "$SUMMARY_FILE"
    return
  fi

  if echo "$run_output" | grep -Eiq "An error occurred:|codec can't decode|UnicodeDecodeError|ConnectionError|SerialException"; then
    echo "$run_output" >>"$RAW_LOG"
    result="RUN_FAIL"
    printf "%-15s %-15s %-12s %-20s %-20s\n" "$param" "$xof" "$result" "cycle=$cycle" "masked_cycle=$masked_cycle" | tee -a "$SUMMARY_FILE"
    return
  fi

  echo "$run_output" >>"$RAW_LOG"

  cycle="$(printf '%s\n' "$run_output" \
    | sed -n 's/^RUBATO_ENCRYPT:[[:space:]]*\([0-9][0-9]*\)[[:space:]]*cycles.*/\1/p' \
    | head -n1)"
  if [[ -z "$cycle" ]]; then
    cycle="NA"
  fi

  masked_cycle="$(printf '%s\n' "$run_output" \
    | sed -n 's/.*MASKED_RUBATO_ENCRYPT:[[:space:]]*\([0-9][0-9]*\)[[:space:]]*cycles.*/\1/p' \
    | head -n1)"
  if [[ -z "$masked_cycle" ]]; then
    masked_cycle="NA"
  fi

  # Prefer Unity summary parsing
  if echo "$run_output" | grep -Eq '[0-9]+ Tests 0 Failures'; then
    result="PASS"
  elif echo "$run_output" | grep -q 'FAIL'; then
    result="FAIL"
  else
    result="UNKNOWN"
  fi

  printf "%-15s %-15s %-12s %-20s %-20s\n" "$param" "$xof" "$result" "cycle=$cycle" "masked_cycle=$masked_cycle" | tee -a "$SUMMARY_FILE"
}

# Cleanup function for auto-generated files
cleanup_generated_files() {
  echo "Cleaning up auto-generated files..."
  rm -f "$SCRIPT_DIR"/*.elf
  rm -f "$SCRIPT_DIR"/*.map
  rm -f "$SCRIPT_DIR"/*.o
  rm -f "$SCRIPT_DIR"/*.d
  rm -f "$SCRIPT_DIR"/build
  find "$SCRIPT_DIR" -name "*.o" -delete
  find "$SCRIPT_DIR" -name "*.d" -delete
}

: > "$RAW_LOG"
printf "%-15s %-15s %-12s %-20s %-20s\n" "PARAM" "XOF" "RESULT" "UNMASKED_CYCLES" "MASKED_CYCLES" > "$SUMMARY_FILE"

for param in "${PARAMS[@]}"; do
  for xof in "${XOFS[@]}"; do
    run_one "$param" "$xof"
  done
done

echo
echo "=========================================="
echo "Test Results Summary"
echo "=========================================="
cat "$SUMMARY_FILE"
echo "=========================================="
echo
echo "Summary: $SUMMARY_FILE"
echo "Raw log: $RAW_LOG"
echo

# Cleanup auto-generated files
cleanup_generated_files

echo "Done!"
