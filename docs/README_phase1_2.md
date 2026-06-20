# Phase 1: Environment Setup

## Overview

Phase 1 establishes the complete cross-compilation and emulation environment needed for the entire project. Before writing a single line of Canny code, we must have a working toolchain that can compile C++ for RISC-V, and an emulator that can run those binaries on our development machine.

## How to Run

```bash
qemu-riscv64 --version
```

---

# Phase 2: Scalar Baseline Pipeline

## Overview

Phase 2 implements the complete Canny edge detection pipeline in clean, readable, scalar C++. This is the **reference implementation** — every optimization in Phases 4–6 must produce identical output to what we build here. Getting this right before touching any vector intrinsics is critical: if the baseline has a bug, we cannot tell whether our RVV code is correct.

---

## Pipeline Stages

### 2.1 Image I/O

We use raw grayscale format: a flat binary file of exactly width × height bytes, where each byte is one pixel (0 = black, 255 = white). No headers, no compression, no library dependencies.

**Key design decision:** Use `aligned_alloc(64, width * height)` instead of `malloc` for image buffers. Aligned memory is required for RVV vector load intrinsics later and helps the compiler vectorize loads and stores now.

---

### 2.2 Gaussian Blur (5×5 Kernel)

We apply a 2D convolution with a 5×5 Gaussian kernel. The integer coefficients sum to **273**.

**Implementation rules:**
- Use integer arithmetic throughout (no floating point)
- Accumulate into `int32_t` to avoid overflow (worst case: 255 × 15 × 25 ≈ 95K)
- Divide by 273 at the end, then clamp result to [0, 255]
- **Boundary handling:** Zero-padding — out-of-bounds pixels are treated as 0

**Template design:** The convolution function is templated on pixel type, accumulator type, and kernel coefficient type. The RVV implementation in Phase 6 becomes a template specialization of this same interface.

---

### 2.3 Sobel Gradient Computation

Two 3×3 convolutions applied to the blurred image — one for horizontal gradient (Gx), one for vertical gradient (Gy). Output is stored as two separate `int16_t` buffers — **Structure of Arrays (SoA)** layout, not interleaved pairs. This is one of the most important data layout decisions in the project:

- **SoA (our choice):** Consecutive Gx values and consecutive Gy values — enables a single vector load in Phase 6
- **AoS (wrong choice):** Interleaved Gx and Gy pairs — would require expensive gather operations to vectorize

> `int16_t` is sufficient: the maximum Sobel output on 8-bit pixels is 4 × 255 = 1020, well within the int16_t range.

---

### 2.4 Gradient Magnitude

We implement **two methods** and compare their output quality:

- **L1 Norm (fast):** `|Gx| + |Gy|` — integer only, no floating point, slight overestimate on diagonal edges but fast and vectorizable
- **L2 Norm (accurate):** `sqrt(Gx² + Gy²)` — mathematically correct, requires floating point or fixed-point square root

**Normalization:** Both methods normalize output to [0, 255] by dividing by the global maximum. This requires two passes — one to find the max, one to normalize.

---

### 2.5 Gradient Direction

We quantize the gradient direction to **four values only**: 0°, 45°, 90°, 135°. No `atan2()` needed — we use integer cross-multiplication instead of floating-point division, which is a standard embedded optimization.

---

## How to Run

```bash
cat src/gaussian.cpp
cat src/sobel.cpp
cat src/nms.cpp 
cat src/hysteresis.cpp
```

---

## Key Design Decisions Summary

| Decision | Choice | Reason |
|----------|--------|--------|
| Image format | Raw grayscale (no headers) | No library dependencies, simple I/O |
| Memory allocation | `aligned_alloc(64, ...)` | Required for RVV vector loads in Phase 6 |
| Kernel arithmetic | Integer only | Avoids floating-point overhead on embedded |
| Boundary handling | Zero-padding | Simplest approach, enables vectorization |
| Gradient storage | SoA (separate Gx, Gy arrays) | Enables single vector load in Phase 6 |
| Gradient type | `int16_t` | Sufficient range (max ±1020), half the memory of int32 |
| Direction quantization | Integer cross-multiply | No `atan2()`, no floating point needed |
| Convolution interface | C++ template | RVV version becomes a template specialization |

---

## Common Pitfalls

- **Forgetting the boundary:** The 2-pixel border around a 5×5 convolution needs zero-padding. If skipped, the entire image border will be wrong and tests will fail.
- **Using `malloc` instead of `aligned_alloc`:** RVV vector load intrinsics require 64-byte aligned memory. Misaligned buffers cause crashes at runtime.
- **Overflow in accumulator:** Accumulating into `int16_t` during Gaussian convolution overflows. Always use `int32_t` for the accumulator.
- **AoS instead of SoA for Gx/Gy:** Interleaved storage makes the Sobel magnitude stage extremely difficult to vectorize efficiently.
