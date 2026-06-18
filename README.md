# Canny Edge Detection on RISC-V with Vector Extension

## Overview
Implementation of Canny Edge Detection targeting RISC-V (rv64gcv) with RVV intrinsic optimization, running on QEMU user-mode emulation.

## Team
| Name | Role |
|------|------|
| Radwa Ahmed | Team Lead, Integration |
| Dana Mohamed | Image I/O, Gaussian Blur, Sobel |
| Roaa Youssry | NMS, Hysteresis |
| Mohamed Abdallah | Phase 4 timing, optimization sweep |
| Nada Tamer | Pipeline testing on QEMU |

## Requirements
- WSL2 (Ubuntu 24.04) on Windows, or Linux natively
- RISC-V GNU Toolchain (built from source with rv64gcv support)
- QEMU (built from source, riscv64-linux-user)
- GoogleTest (for host-side testing)

## Build Instructions

### 1. Clone the repository
```bash
git clone https://github.com/Radwa-Ahmedx3/Canny-Edge-Detector-RISCV.git
cd Canny-Edge-Detector-RISCV
```

### 2. Build host binary
```bash
mkdir -p build-host build-rv
g++ -I include src/main.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -o build-host/canny_host
```

### 3. Run on host
```bash
./build-host/canny_host
```

### 4. Cross-compile for RISC-V
```bash
riscv64-unknown-elf-g++ -O3 -march=rv64gcv -mabi=lp64d -static -I include src/main_qemu.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -o build-rv/canny_rv
```

### 5. Run on QEMU
```bash
qemu-riscv64 -cpu rv64,v=true,vlen=128 ./build-rv/canny_rv
qemu-riscv64 -cpu rv64,v=true,vlen=256 ./build-rv/canny_rv
qemu-riscv64 -cpu rv64,v=true,vlen=512 ./build-rv/canny_rv
```

### 6. Run GoogleTest suite
```bash
g++ -I include -I ~/gtest-install/include tests/pipeline_test.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp src/nms.cpp src/hysteresis.cpp -L ~/gtest-install/lib -lgtest -lgtest_main -lpthread -o build-host/pipeline_tests
./build-host/pipeline_tests
```

### 7. Run RVV intrinsics timing
```bash
riscv64-unknown-elf-g++ -O3 -march=rv64gcv -mabi=lp64d -static -I include src/main_rvv_timing.cpp src/image.cpp src/gaussian.cpp src/gaussian_rvv.cpp src/sobel.cpp src/sobel_rvv.cpp -o build-rv/timing_rvv
qemu-riscv64 -cpu rv64,v=true,vlen=128 ./build-rv/timing_rvv
```

### 8. Run RVV equivalence test
```bash
riscv64-unknown-elf-g++ -O3 -march=rv64gcv -mabi=lp64d -static -I include src/test_rvv_equivalence.cpp src/image.cpp src/gaussian.cpp src/gaussian_rvv.cpp src/sobel.cpp src/sobel_rvv.cpp -o build-rv/test_rvv
qemu-riscv64 -cpu rv64,v=true,vlen=128 ./build-rv/test_rvv
qemu-riscv64 -cpu rv64,v=true,vlen=256 ./build-rv/test_rvv
qemu-riscv64 -cpu rv64,v=true,vlen=512 ./build-rv/test_rvv
```

## Project Structure
```
include/    - Header files
src/        - Source files
tests/      - GoogleTest and QEMU test files
build-host/ - Host compiled binaries
build-rv/   - RISC-V compiled binaries
docs/       - Documentation and AI usage log
```

## Optimization Results

### Compiler Sweep (QEMU VLEN=128)
| Stage | -O0 | -O2 | -O3 | -Os | -Ofast |
|-------|-----|-----|-----|-----|--------|
| Gaussian | 17.51ms | 11.20ms | 10.02ms | 11.90ms | 11.38ms |
| Sobel | 8.63ms | 6.23ms | 5.36ms | 6.06ms | 6.66ms |

### RVV Intrinsics Speedup
| Stage | Scalar -O3 | RVV VLEN=128 | RVV VLEN=512 |
|-------|-----------|--------------|--------------|
| Gaussian | 9.85ms | 3.04ms (3.24x) | 2.52ms (4.33x) |
| Sobel | 8.43ms | 4.30ms (1.96x) | 4.29ms (1.94x) |
