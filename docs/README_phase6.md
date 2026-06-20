# Phase 6: RVV Intrinsic Optimization

## Why We Perform Phase 6

Phase 6 is the engineering climax of the project, where we transition from compiler-dependent scalar code to hardware-accelerated performance. Using the bottleneck data gathered during Phase 5, we systematically rewrite our hottest pipeline kernels — specifically **Gaussian Convolution** and **Sobel Magnitude** — using hand-optimized RISC-V Vector (RVV) intrinsics. This allows us to achieve maximum processing throughput by exploiting parallel data execution directly on the underlying RISC-V vector hardware.

---

## Core RVV Concepts Implemented

Writing effective RVV code requires a shift from traditional fixed-width SIMD models to a flexible, hardware-agnostic paradigm:

### 1. Vector-Length Agnosticism (VLA) & Strip-Mining
Unlike architecture-specific vector sets (e.g., x86 AVX or ARM NEON), RVV code never hardcodes the vector length. We use `__riscv_vsetvl_e*` loop constructs to query the hardware capability dynamically at runtime, processing data in variable "chunks" until the image loop is completed. This ensures that our exact same compiled binary executes flawlessly across different hardware widths.

```cpp
for (int i = 0; i < n; i += vl) {
    vl = __riscv_vsetvl_e32m1(n - i);
    // process vl elements
}
```

### 2. LMUL (Length MULtiplier) Selection
We manage the tradeoff between vector length and register availability by grouping multiple physical vector registers into single logical registers. We run an experimental sweep across `LMUL=1`, `LMUL=2`, and `LMUL=4` to pinpoint the ideal sweet spot before performance degrades due to register pressure spilling.

### 3. Data Widening Operations
To prevent arithmetic overflow when multiplying 8-bit pixels with 16-bit filter weights, we utilize widening math instructions such as `__riscv_vwmul`. This requires careful precision mapping, as widening operations automatically double the active LMUL tier down the processing chain.

### 4. Vector Reductions
For computing global normalization factors in the Sobel magnitude phase, we use vector reduction intrinsics like `__riscv_vredmax`, collapsing an entire vector register down into a single scalar output element, extracted with `__riscv_vmv_x_s`.

---

## Phase 6.2 — LMUL Sweep (Gaussian Convolution)

We sweep LMUL values at three VLEN settings to find the optimal register grouping.

![LMUL Sweep Results](img_phase6_lmul_sweep.jpeg)

| VLEN | LMUL=1 | LMUL=2 (baseline) | LMUL=4 | Fixed-Point |
|------|--------|-------------------|--------|-------------|
| 128 | 14.69 ms | 9.89 ms | **9.76 ms** ← faster | 10.11 ms |
| 256 | 10.13 ms | 9.68 ms | **4.08 ms** ← faster | 9.53 ms ← fastest |
| 512 | 9.74 ms | 4.04 ms | **2.91 ms** ← faster | 4.02 ms ← fastest |

**Key Insight:** LMUL=4 consistently outperforms LMUL=1 and LMUL=2 because the Gaussian kernel accumulates many intermediate values — giving the processor more data to work with per iteration without hitting register spill. Performance scales clearly with VLEN, confirming true vector-length agnosticism.

---

## Phase 6.3 — Sobel Magnitude RVV (L1 Norm)

RVV implementation of the Sobel L1 magnitude: `|Gx| + |Gy|`, using `__riscv_vredmax` for global max and `__riscv_vmv_x_s` to extract the scalar result for normalization.

![Sobel RVV Results](img_phase6_sobel_rvv.jpeg)

| VLEN | Scalar | RVV | Speedup |
|------|--------|-----|---------|
| 128 | 4.05 ms | 2.97 ms | **26.55% faster** |
| 256 | 4.00 ms | 2.30 ms | **42.54% faster** |
| 512 | 3.87 ms | 2.17 ms | **43.92% faster** |

**Key Insight:** Speedup grows with VLEN — at VLEN=512, the RVV version processes 4× more elements per instruction cycle than at VLEN=128, which directly translates to nearly 44% runtime reduction.

---

## Phase 6.4 — Gaussian Full Vectorization

Full RVV vectorization of the 5×5 Gaussian convolution, including interior and boundary pixels.

![Gaussian Full Vectorization](img_phase6_gaussian_full.jpeg)

| Version | Time | Result |
|---------|------|--------|
| Scalar (baseline) | 8.68 ms | — |
| RVV | 9.96 ms | -14.75% |

**Discussion:** The full Gaussian vectorization shows a slight regression compared to scalar at this VLEN. This is expected: the 5×5 kernel requires managing a complex chain of widening operations (u8 → i16 → i32) with LMUL doubling at each stage, increasing register pressure. The overhead of the widening chain and boundary handling partially offsets the gains from parallelism. This result validates the Phase 5 finding that the **LMUL sweep (Phase 6.2)** is the correct approach for Gaussian — using LMUL=4 with a pre-padded image gives superior results.

---

## Phase 6.5 — Sobel Full Vectorization

Full RVV vectorization of the Sobel magnitude with complete boundary handling.

![Sobel Full Vectorization](img_phase6_sobel_full.jpeg)

| Version | Time | Result |
|---------|------|--------|
| Scalar (baseline) | 4.02 ms | — |
| RVV | 3.12 ms | **22.20% faster** |

**Key Insight:** Unlike Gaussian, Sobel full vectorization achieves a clean speedup. The simpler data flow (int16_t inputs, abs + add, no deep widening chain) allows the RVV version to benefit from parallelism without the overhead seen in Gaussian.

---

## Phase 6.6 — VLEN Sweep: Vector-Length Agnosticism Verification

The final test verifies that our RVV pipeline produces **identical output** at VLEN=128, 256, and 512 — the defining requirement of correct RVV code.

![VLEN Sweep Results](img_phase6_vlen_sweep.jpeg)

| VLEN | RVV Pipeline Time | VLA Verified |
|------|-------------------|--------------|
| 128 | 12.66 ms | ✓ |
| 256 | 11.77 ms | ✓ |
| 512 | 5.99 ms | ✓ |

**VLA Verification Checklist (all passed):**
- ✓ Strip-mining with `vsetvl` adapts to hardware VLEN
- ✓ No hardcoded vector length in code
- ✓ Output correctness: **IDENTICAL** at VLEN=128/256/512
- ✓ Performance scales: More elements/iteration at higher VLEN

**Key Insight:** The 2.1× speedup from VLEN=128 to VLEN=512 confirms that our code is truly vector-length agnostic — the same binary automatically exploits wider hardware without recompilation.

---

## Overall Phase 6 Summary

| Experiment | Best Result |
|------------|------------|
| LMUL Sweep (Gaussian) | LMUL=4 at VLEN=512 → **2.91 ms** (vs 9.74 ms at LMUL=1) |
| Sobel RVV (L1) | VLEN=512 → **43.92% faster** than scalar |
| Gaussian Full Vec | Slight regression — widening chain overhead |
| Sobel Full Vec | **22.20% faster** than scalar |
| VLEN Sweep | VLEN=512 → **5.99 ms** pipeline (2.1× faster than VLEN=128) |

---

## How to Run

```bash
# LMUL sweep for Gaussian convolution (at VLEN=128/256/512)
make phase6-2

# Sobel Magnitude RVV vs Scalar (at VLEN=128/256/512)
make phase6-3

# Gaussian full vectorization at VLEN=128
make phase6-4-run

# Sobel full vectorization at VLEN=128
make phase6-5-run

# Full pipeline VLEN sweep (128/256/512)
make phase6-6-run

# Run all Phase 6 targets at once
make phase6-4-5-6
```
