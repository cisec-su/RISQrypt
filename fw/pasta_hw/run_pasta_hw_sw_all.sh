#!/usr/bin/env bash
set -euo pipefail

PORT="${1:-/dev/ttyUSB1}"
ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
FW_HW_DIR="$ROOT_DIR/fw/pasta_hw"
FW_SW_DIR="$ROOT_DIR/fw/pasta_sw"
CODE_SIZE_HW_DIR="$ROOT_DIR/fw/benchmark/code_size/pasta_code_size"
CODE_SIZE_SW_DIR="$ROOT_DIR/fw/benchmark/code_size/pasta_sw_code_size"
SIZE_TOOL="/opt/riscv/bin/riscv64-unknown-elf-size"

if [[ ! -x "$SIZE_TOOL" ]]; then
  echo "Missing size tool: $SIZE_TOOL"
  exit 1
fi

extract_hw_cycles() {
  local out="$1"
  local unmasked masked
  unmasked=$(printf "%s\n" "$out" | awk '/PASTA_ENCRYPT:/ {gsub(/,/, "", $(NF-1)); print $(NF-1); exit}')
  masked=$(printf "%s\n" "$out" | awk '/PASTA_ENCRYPT:/ {gsub(/,/, "", $(NF-1)); c++; if (c==2) {print $(NF-1); exit}}')
  [[ -z "$unmasked" ]] && unmasked="N/A" || unmasked="$unmasked cycles"
  [[ -z "$masked" ]] && masked="N/A" || masked="$masked cycles"
  printf "%s|%s" "$unmasked" "$masked"
}

extract_sw_cycles() {
  local out="$1"
  local unmasked masked
  unmasked=$(printf "%s\n" "$out" | awk '/PASTA_SOFT_ENCRYPT:/ {gsub(/,/, "", $(NF-1)); print $(NF-1); exit}')
  masked=$(printf "%s\n" "$out" | awk '/MASKED_PASTA_SOFT_ENCRYPT:/ {gsub(/,/, "", $(NF-1)); print $(NF-1); exit}')

  # Some firmware variants print generic PASTA_ENCRYPT labels instead of SOFT labels.
  if [[ -z "$unmasked" ]]; then
    unmasked=$(printf "%s\n" "$out" | awk '/PASTA_ENCRYPT:/ {gsub(/,/, "", $(NF-1)); print $(NF-1); exit}')
  fi
  if [[ -z "$masked" ]]; then
    masked=$(printf "%s\n" "$out" | awk '/PASTA_ENCRYPT:/ {gsub(/,/, "", $(NF-1)); c++; if (c==2) {print $(NF-1); exit}}')
  fi

  [[ -z "$unmasked" ]] && unmasked="N/A" || unmasked="$unmasked cycles"
  [[ -z "$masked" ]] && masked="N/A" || masked="$masked cycles"
  printf "%s|%s" "$unmasked" "$masked"
}

build_and_size() {
  local dir="$1"
  local elf="$2"
  local cflags="$3"
  make -C "$dir" clean >/dev/null 2>&1 || true
  make -C "$dir" CFLAGS="$cflags" >/tmp/$(basename "$dir")_build.log 2>&1
  "$SIZE_TOOL" -B "$dir/$elf" | awk 'NR==2 {printf "%s\t%s\t%s\t%s\t%s", $1, $2, $3, $4, $5}'
}

print_tsv_table() {
  local rows="$1"
  if command -v column >/dev/null 2>&1; then
    printf "%s\n" "$rows" | column -t -s $'\t'
  else
    printf "%s\n" "$rows"
  fi
}

echo "=== Cycles ==="
printf "%-20s %-14s %-14s %-28s %-28s\n" "target" "mem_opt" "rej_samp" "unmasked" "masked"

# runtime: pasta_hw (4 combinations)
for rej in 0 1; do
  for mem in 0 1; do
    defs="-DBUSY_CHECK_DIS"
    [[ "$rej" -eq 1 ]] && defs="$defs -DREJ_SAMP_DIS"
    [[ "$mem" -eq 1 ]] && defs="$defs -DMEMORY_OPT_DIS"

    [[ "$rej" -eq 1 ]] && rej_state="DIS" || rej_state="EN"
    [[ "$mem" -eq 1 ]] && mem_state="DIS" || mem_state="EN"

    make -C "$FW_HW_DIR" clean >/dev/null 2>&1 || true
    make -C "$FW_HW_DIR" CFLAGS="$defs" >/tmp/pasta_hw_build.log 2>&1
    out=$(cd "$FW_HW_DIR" && python3 "$ROOT_DIR/sdk/toolchain/bootloader.py" -f pasta_hw -p "$PORT" 2>&1)
    cycles=$(extract_hw_cycles "$out")
    unmasked=${cycles%%|*}
    masked=${cycles##*|}

    printf "%-20s %-14s %-14s %-28s %-28s\n" "pasta_hw" "${mem_state}" "${rej_state}" "$unmasked" "$masked"
  done
done

# runtime: pasta_sw (single configuration)
make -C "$FW_SW_DIR" clean >/dev/null 2>&1 || true
make -C "$FW_SW_DIR" >/tmp/pasta_sw_build.log 2>&1
out=$(cd "$FW_SW_DIR" && python3 "$ROOT_DIR/sdk/toolchain/bootloader.py" -f pasta_sw -p "$PORT" 2>&1)
cycles=$(extract_sw_cycles "$out")
unmasked=${cycles%%|*}
masked=${cycles##*|}
printf "%-20s %-14s %-14s %-28s %-28s\n" "pasta_sw" "N/A" "EN" "$unmasked" "$masked"

# code size: pasta_hw (masked/unmasked)
echo
echo "=== Code Size ==="
rows=$'target\tvariant\ttext\tdata\tbss\tdec\thex'

hw_unmasked_size=$(build_and_size "$CODE_SIZE_HW_DIR" "pasta_code_size.elf" "-UMASKING_EN")
hw_masked_size=$(build_and_size "$CODE_SIZE_HW_DIR" "pasta_code_size.elf" "-DMASKING_EN")
IFS=$'\t' read -r hw_u_text hw_u_data hw_u_bss hw_u_dec hw_u_hex <<< "$hw_unmasked_size"
IFS=$'\t' read -r hw_m_text hw_m_data hw_m_bss hw_m_dec hw_m_hex <<< "$hw_masked_size"
printf -v row "code_size_hw\tunmasked\t%s\t%s\t%s\t%s\t%s" "$hw_u_text" "$hw_u_data" "$hw_u_bss" "$hw_u_dec" "$hw_u_hex"
rows+=$'\n'"$row"
printf -v row "code_size_hw\tmasked\t%s\t%s\t%s\t%s\t%s" "$hw_m_text" "$hw_m_data" "$hw_m_bss" "$hw_m_dec" "$hw_m_hex"
rows+=$'\n'"$row"

# code size: pasta_sw (masked/unmasked)
sw_unmasked_size=$(build_and_size "$CODE_SIZE_SW_DIR" "pasta_sw_code_size.elf" "-UMASKING_EN")
sw_masked_size=$(build_and_size "$CODE_SIZE_SW_DIR" "pasta_sw_code_size.elf" "-DMASKING_EN")
IFS=$'\t' read -r sw_u_text sw_u_data sw_u_bss sw_u_dec sw_u_hex <<< "$sw_unmasked_size"
IFS=$'\t' read -r sw_m_text sw_m_data sw_m_bss sw_m_dec sw_m_hex <<< "$sw_masked_size"
printf -v row "code_size_sw\tunmasked\t%s\t%s\t%s\t%s\t%s" "$sw_u_text" "$sw_u_data" "$sw_u_bss" "$sw_u_dec" "$sw_u_hex"
rows+=$'\n'"$row"
printf -v row "code_size_sw\tmasked\t%s\t%s\t%s\t%s\t%s" "$sw_m_text" "$sw_m_data" "$sw_m_bss" "$sw_m_dec" "$sw_m_hex"
rows+=$'\n'"$row"

print_tsv_table "$rows"

# cleanup: remove all build artifacts
echo
echo "Cleaning build artifacts..."
make -C "$FW_HW_DIR" clean >/dev/null 2>&1 || true
make -C "$FW_SW_DIR" clean >/dev/null 2>&1 || true
make -C "$CODE_SIZE_HW_DIR" clean >/dev/null 2>&1 || true
make -C "$CODE_SIZE_SW_DIR" clean >/dev/null 2>&1 || true
echo "Cleanup complete."
