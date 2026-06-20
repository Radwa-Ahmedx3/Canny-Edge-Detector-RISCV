# Phase 5: Profiling and Hotspot Identification

## Why We Perform Phase 5

The core objective of Phase 5 is to transition from generic optimizations to data-driven targeting using **Amdahl's Law**. Rather than wasting valuable engineering time trying to optimize the entire pipeline, we isolate and measure the precise execution time of each individual stage. This metric tells us exactly where the performance bottlenecks (hotspots) live, ensuring we only invest our RISC-V Vector (RVV) intrinsic optimization efforts where they will yield a massive, meaningful speedup.

---

## Profiling Methodology

Each pipeline stage is wrapped in `clock_gettime(CLOCK_MONOTONIC, ...)` timing calls. The pipeline runs on QEMU (`qemu-riscv64 -cpu rv64,v=true,vlen=128`) compiled with `-Os`, and timing is averaged over **100 runs** to eliminate noise (since QEMU is not cycle-accurate, wall-clock averaging gives stable relative comparisons).

The command used:
```bash
make phase5
# Compiles with: riscv64-unknown-elf-g++ -march=rv64gcv -mabi=lp64d -static -Os
# Runs with:     qemu-riscv64 -cpu rv64,v=true,vlen=128 ./phase5_profile
```

---

## Results

### Per-Stage Timing Breakdown

![Phase 5 Profiling Results](img_phase5_profiling.jpeg)

| Pipeline Stage | Time (ms) | Percentage |
|---------------|-----------|------------|
| **Gaussian Blur** | 9.3 ms | **43.2%** |
| **Sobel Gradient** | 7.3 ms | **34.0%** |
| NMS | 3.1 ms | 14.2% |
| Hysteresis | 1.9 ms | 8.7% |
| **Total** | **21.6 ms** | **100%** |

---

## Analysis

### The Two Hotspots: Gaussian + Sobel = 77.2% of Total Runtime

The profiling data is unambiguous. Two stages dominate the runtime:

**1. Gaussian Blur (43.2%)**
This is the most expensive stage. It performs a full 5×5 convolution over every pixel — 25 multiply-accumulate operations per output pixel. The inner loop is compute-bound and operates on contiguous memory, making it an ideal candidate for RVV vectorization.

**2. Sobel Gradient (34.0%)**
Two separate 3×3 convolutions (Gx and Gy) applied to the blurred image. The Structure-of-Arrays memory layout we chose in Phase 2 makes this stage well-suited for vector loads.

### The Two Cold Spots: NMS + Hysteresis = 22.9%

**NMS (14.2%)** and **Hysteresis (8.7%)** together account for less than a quarter of the runtime. These stages involve complex conditional logic that is difficult to vectorize and would yield limited returns even if we succeeded.

---

## Decision: What to Optimize in Phase 6

Based on this data, our Phase 6 RVV effort will focus **exclusively** on:

| Stage | Decision | Reason |
|-------|----------|--------|
| ✅ Gaussian Blur | **Optimize** | 43.2% of runtime — highest impact, regular memory access, pure compute |
| ✅ Sobel Magnitude | **Optimize** | 34.0% of runtime — SoA layout enables clean vector loads |
| ❌ NMS | **Skip** | 14.2% — complex control flow, low return on effort |
| ❌ Hysteresis | **Skip** | 8.7% — inherently sequential, not worth vectorizing |

> **Do not optimize what you have not measured.**
> The profiling data from Phase 5 is the direct justification for every RVV intrinsic written in Phase 6.

---

## How to Run

```bash
make phase5
```
