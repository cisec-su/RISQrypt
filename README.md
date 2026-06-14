# RISQrypt  
**Fast, Secure and Agile HW/SW Co-Design for Post-Quantum Cryptography**

RISQrypt is a hardware/software co-design framework targeting efficient implementations of post-quantum cryptographic (PQC) algorithms with masking.

---

## 🚀 Supported Algorithms

- CRYSTALS-Kyber (ML-KEM)
- CRYSTALS-Dilithium (ML-DSA)
- SPHINCS+ (SLH-DSA)
- PASTA-3 (Transciphering algorithm for Fully Homomorphic Encryption)

## 🚀 Coming Soon
- Falcon (FN-DSA)
- Rubato (Transciphering algorithm for Fully Homomorphic Encryption)

---

## 🛠 FPGA Programming

RISQrypt targets the **Nexys A7-100T FPGA** board.

### 1️⃣ Generate Bitstream (Vivado)

Run synthesis and implementation in Vivado:
```bash
./vivado/scripts/program.sh -g
```
The generated bitstream should be:

```
vivado/risqrypt.runs/impl_1/fpga_top.bit
```

### 2️⃣ Program FPGA

Use `Vivado` or `openFPGALoader` to flash the bitstream.

Vivado:
```bash
./vivado/scripts/program.sh -p
```
openFPGALoader:
```bash
openFPGALoader -b nexys_a7_100 vivado/risqrypt.runs/impl_1/fpga_top.bit
```

Make sure:
- The board is connected via USB-JTAG
- `openFPGALoader` is installed
- You have permission to access USB devices

---

## 🧪 Running Tests

All firmware is built using `make` and uploaded via UART using the RISQrypt bootloader.

---

### 🔐 Kyber Test

```bash
cd fw/test/kyber_test
make
cd ../../../
python3 sdk/toolchain/bootloader.py -f fw/test/kyber_test/kyber_test
```

---

### ✍️ Dilithium Test

```bash
cd fw/test/dilithium_test
make
cd ../../../
python3 sdk/toolchain/bootloader.py -f fw/test/dilithium_test/dilithium_test
```

### 🌲 SPHINCS+ Test

```bash
cd fw/test/sphincs_test
make
cd ../../../
python3 sdk/toolchain/bootloader.py -f fw/test/sphincs_test/sphincs_test
```

---

### 🔐 Pasta Test

```bash
cd fw/test/pasta_test
make
cd ../../../
python3 sdk/toolchain/bootloader.py -f fw/test/pasta_test/pasta_test
```

---

## ⏱ Benchmarking

Performance benchmarks measure execution time of cryptographic operations.

---

### 📊 Kyber Time Benchmark

```bash
cd fw/benchmark/time_perf/kyber_time_perf
make
cd ../../../../
python3 sdk/toolchain/bootloader.py -f fw/benchmark/time_perf/kyber_time_perf/kyber_time_perf
```

---

### 📊 Dilithium Time Benchmark

```bash
cd fw/benchmark/time_perf/dilithium_time_perf
make
cd ../../../../
python3 sdk/toolchain/bootloader.py -f fw/benchmark/time_perf/dilithium_time_perf/dilithium_time_perf
```

---

### 📊 SPHINCS+ Time Benchmark

```bash
cd fw/benchmark/time_perf/sphincs_time_perf
make
cd ../../../../
python3 sdk/toolchain/bootloader.py -f fw/benchmark/time_perf/sphincs_time_perf/sphincs_time_perf
```

---

### 📊 Pasta Time Benchmark

```bash
cd fw/benchmark/time_perf/pasta_time_perf
make
cd ../../../../
python3 sdk/toolchain/bootloader.py -f fw/benchmark/time_perf/pasta_time_perf/pasta_time_perf
```


## 👤

For any questions: Tolun Tosun (toluntosun@sabanciuniv.edu)
