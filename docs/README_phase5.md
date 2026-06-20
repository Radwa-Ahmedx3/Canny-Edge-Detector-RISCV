# Phase 5: Profiling and Hotspot Identification

## Why We Perform Phase 5

Before writing a single line of RVV intrinsics, we must answer one critical question:  
**Where does the time actually go?**

This is the core principle of performance engineering: **measure first, optimize second**. Without profiling data, we risk spending hours hand-optimizing a function that contributes 5% of the total runtime — achieving almost nothing. Phase 5 gives us the data to make smart decisions about where manual RVV intrinsics will have the greatest impact.

This is **Amdahl's Law** in practice: the maximum speedup of the entire pipeline is limited by the fraction of time spent in the parts we *don't* optimize.

---

## Profiling Methodology

Each pipeline stage is wrapped in `clock_gettime(CLOCK_MONOTONIC, ...)` timing calls. The pipeline runs on QEMU (`qemu-riscv64 -cpu rv64,v=true,vlen=128`) compiled with `-O3`, and timing is averaged over **100 runs** to eliminate noise (since QEMU is not cycle-accurate, wall-clock averaging gives stable relative comparisons).

The command used:
```bash
make phase5
# Compiles with: riscv64-unknown-elf-g++ -march=rv64gcv -mabi=lp64d -static -O3
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
This is the most expensive stage. It performs a full 5×5 convolution over every pixel — 25 multiply-accumulate operations per output pixel. On a 512×512 image, that is over 6.5 million MAC operations. The inner loop is compute-bound and operates on contiguous memory, making it an ideal candidate for RVV vectorization.

**2. Sobel Gradient (34.0%)**  
Two separate 3×3 convolutions (Gx and Gy) applied to the blurred image. While the kernel is smaller (9 MACs per pixel vs. 25), it runs twice and produces 16-bit signed outputs. The Structure-of-Arrays memory layout we chose in Phase 2 makes this stage well-suited for vector loads.

### The Two Cold Spots: NMS + Hysteresis = 22.9%

**NMS (14.2%)** and **Hysteresis (8.7%)** together account for less than a quarter of the runtime. These stages involve complex conditional logic (comparing neighbors, following edge chains) that is difficult to vectorize and would yield limited returns even if we succeeded.

---

## Decision: What to Optimize in Phase 6

Based on this data, our Phase 6 RVV effort will focus **exclusively** on:

| Stage | Why |
|-------|-----|
| ✅ Gaussian Blur | 43.2% of runtime — highest impact, regular memory access pattern, pure compute |
| ✅ Sobel Magnitude | 34.0% of runtime — SoA layout enables clean vector loads, abs + add is simple to vectorize |
| ❌ NMS | 14.2% — complex control flow, difficult to vectorize, low return on effort |
| ❌ Hysteresis | 8.7% — recursive edge tracing, inherently sequential, not worth vectorizing |

Optimizing only Gaussian and Sobel targets **77.2% of the total runtime**. If our RVV implementation achieves a 4× speedup on these two stages, the theoretical overall speedup is:

$$S = \frac{1}{(1 - 0.772) + \frac{0.772}{4}} \approx 2.9\times$$

This is a realistic and meaningful target for Phase 6.

---

## Key Takeaway

> **Do not optimize what you have not measured.**  
> The profiling data from Phase 5 is the direct justification for every RVV intrinsic written in Phase 6. Without this data, optimization is guesswork.
