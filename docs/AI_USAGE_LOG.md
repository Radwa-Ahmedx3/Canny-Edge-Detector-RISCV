# AI Usage Log

## Entry 1
**Question asked:** How to implement RVV intrinsics for Gaussian blur using separable filter approach?
**AI suggested:** Use two 1D passes with kernel [1,4,7,4,1] instead of 2D 5x5 convolution, reducing multiply-accumulate operations from 25 to 10 per pixel.
**What we changed:** We verified the LMUL chain (e8m2 -> u16m4 -> u32m8) and tested equivalence against scalar at VLEN=128/256/512.
**What we learned:** Algorithmic improvement (separable filter) gave more speedup than intrinsics alone. Naive vectorization with temp buffers was 6x SLOWER than scalar.

## Entry 2
**Question asked:** Why does the compiler fail to auto-vectorize our Gaussian and Sobel loops at -O3?
**AI suggested:** Two reasons: boundary condition checks (if px>=0 && px<w) and C++ exception handling on function calls prevent auto-vectorization.
**What we changed:** We ran -fopt-info-vec-all to confirm 0 loops vectorized, then used this as justification for manual RVV intrinsics.
**What we learned:** Code structure affects vectorizability. Removing boundary checks (using padding) can allow auto-vectorization.

## Entry 3
**Question asked:** How to implement syscalls.cpp to enable file I/O on QEMU with bare-metal toolchain?
**AI suggested:** Provide newlib syscall stubs (_open, _close, _read, _write) using RISC-V ecall instructions that QEMU intercepts as Linux syscalls.
**What we changed:** We discovered that newlib O_CREAT/O_TRUNC flag values differ from Linux, requiring flag translation in _open. We also removed _fstat Linux struct conflict.
**What we learned:** Bare-metal newlib uses different O_ flag values than Linux. Direct syscall implementation requires careful flag translation.

## Entry 4
**Question asked:** How to implement Sobel RVV that is actually faster than scalar?
**AI suggested:** Vectorize Gx and Gy computation directly using 8 shifted pixel loads, computing gradients in int16 vectors.
**What we changed:** First attempt (two-pass with scalar Gx/Gy + vector magnitude) was slower. Second attempt vectorizing Gx/Gy directly achieved 1.42x-1.96x speedup.
**What we learned:** Vectorizing only the magnitude while keeping Gx/Gy scalar adds overhead that negates benefits. The key is vectorizing the actual computation bottleneck.

## Entry 5
**Question asked:** How to compare L1 vs L2 gradient magnitude norms?
**AI suggested:** Implement sobelGradientL2() using sqrtf(gx*gx + gy*gy) and compare pixel-by-pixel with timing over 100 runs.
**What we changed:** Added the comparison to sobel.cpp and created compare_l1_l2.cpp. Results showed L2 is 5.79% slower with max pixel difference of 63.
**What we learned:** L1 norm is preferred for embedded systems due to integer-only arithmetic and negligible quality difference for edge detection purposes.
