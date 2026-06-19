#include "gaussian.hpp"
#include <riscv_vector.h>
#include <cstdint>

static const int krvv[5][5] = {{1,4,7,4,1},{4,16,26,16,4},{7,26,41,26,7},{4,16,26,16,4},{1,4,7,4,1}};

// LMUL=2: chosen because 32-bit accumulators with 25 multiply-accumulate ops
// fit without register spilling. At LMUL=2 we have 16 logical registers.
// If VLEN changes: vl changes automatically, same code works at VLEN=128/256/512.
Image gaussianBlurRVV(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();
    int pw = w + 4;
    int ph = h + 4;
    uint8_t* padded = new uint8_t[pw * ph]();
    const uint8_t* src = input.getConstData();
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            padded[(y+2)*pw+(x+2)] = src[y*w+x];
    uint8_t* out = new uint8_t[w * h];
    for (int y = 0; y < h; y++) {
        int x = 0;
        while (x < w) {
            // (1) Set vector length for 32-bit elements, LMUL=2
            // (2) LMUL=2 balances throughput vs register pressure for 25-op kernel
            // (3) vl scales with VLEN: VLEN=128->4 elems, VLEN=256->8, VLEN=512->16
            size_t vl = __riscv_vsetvl_e32m2(w - x);

            // (1) Initialize accumulator vector to 0
            // (2) Must be i32m2 to match widened pixel type after u8->u16->i32
            // (3) Number of elements processed changes with VLEN, not the logic
            vint32m2_t acc = __riscv_vmv_v_x_i32m2(0, vl);

            for (int ky = 0; ky < 5; ky++) {
                for (int kx = 0; kx < 5; kx++) {
                    const uint8_t* p = &padded[(y+ky)*pw+(x+kx)];

                    // (1) Load vl pixels as unsigned 8-bit (mf2 = LMUL/4 for u8)
                    // (2) mf2 because widening u8->u16 produces m1, then u16->i32 produces m2
                    // (3) At any VLEN, loads exactly vl consecutive bytes
                    vuint8mf2_t u8 = __riscv_vle8_v_u8mf2(p, vl);

                    // (1) Widen u8 -> u16 (zero-extend, doubles element width)
                    // (2) Widening doubles LMUL: mf2 -> m1
                    // (3) LMUL chain is fixed regardless of VLEN
                    vuint16m1_t u16 = __riscv_vwcvtu_x_x_v_u16m1(u8, vl);

                    // (1) Reinterpret u16 as i16 (no instruction, just type change)
                    // (2) Needed because vwcvt (signed widen) requires signed input
                    // (3) No runtime cost, compile-time type cast only
                    vint16m1_t s16 = __riscv_vreinterpret_v_u16m1_i16m1(u16);

                    // (1) Widen i16 -> i32 (sign-extend, doubles element width)
                    // (2) Widening doubles LMUL: m1 -> m2, matches accumulator type
                    // (3) Same widening chain works at all VLEN values
                    vint32m2_t i32 = __riscv_vwcvt_x_x_v_i32m2(s16, vl);

                    // (1) Multiply-accumulate: acc += i32 * krvv[ky][kx] (scalar)
                    // (2) vmacc_vx multiplies vector by scalar coefficient efficiently
                    // (3) 25 iterations total (5x5 kernel), all use same vl
                    acc = __riscv_vmacc_vx_i32m2(acc, krvv[ky][kx], i32, vl);
                }
            }

            // (1) Divide each element by 273 (Gaussian kernel sum normalization)
            // (2) Integer division is expensive; fixed-point alt: *9709>>21 ~= /273
            // (3) vl elements divided in parallel regardless of VLEN
            vint32m2_t divided = __riscv_vdiv_vx_i32m2(acc, 273, vl);

            // (1) Clamp negative values to 0 (shouldn't occur with u8 input)
            // (2) Ensures safe narrowing to u8 in next steps
            // (3) Element-wise max, works at any VLEN
            vint32m2_t clamped = __riscv_vmax_vx_i32m2(divided, 0, vl);

            // (1) Reinterpret i32 as u32 for unsigned narrowing instructions
            // (2) vnclipu requires unsigned input
            // (3) No runtime cost, compile-time type cast
            vuint32m2_t uclamped = __riscv_vreinterpret_v_i32m2_u32m2(clamped);

            // (1) Narrow u32 -> u16 with unsigned clipping (saturate at 65535)
            // (2) LMUL halves: m2 -> m1, shift=0 means no right shift
            // (3) RDN rounding mode: round-down (floor)
            vuint16m1_t n16 = __riscv_vnclipu_wx_u16m1(uclamped, 0, __RISCV_VXRM_RDN, vl);

            // (1) Narrow u16 -> u8 with unsigned clipping (saturate at 255)
            // (2) LMUL halves: m1 -> mf2, completing the narrowing chain
            // (3) Final pixel values guaranteed in [0,255]
            vuint8mf2_t n8 = __riscv_vnclipu_wx_u8mf2(n16, 0, __RISCV_VXRM_RDN, vl);

            // (1) Store vl output pixels to memory
            // (2) mf2 matches the u8 output type
            // (3) Advances by vl each iteration - VLA strip-mining pattern
            __riscv_vse8_v_u8mf2(&out[y*w+x], n8, vl);

            x += vl;
        }
    }
    Image result(w, h);
    uint8_t* d = result.getData();
    for (int i = 0; i < w*h; i++) d[i] = out[i];
    delete[] padded;
    delete[] out;
    return result;
}

// LMUL=1 version: fewer elements per iteration, less register pressure
// Slower than m2 because loop overhead dominates for 5x5 kernel
Image gaussianBlurRVV_m1(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();
    int pw = w + 4;
    int ph = h + 4;
    uint8_t* padded = new uint8_t[pw * ph]();
    const uint8_t* src = input.getConstData();
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            padded[(y+2)*pw+(x+2)] = src[y*w+x];
    uint8_t* out = new uint8_t[w * h];
    for (int y = 0; y < h; y++) {
        int x = 0;
        while (x < w) {
            // LMUL=1: half elements vs m2 per iteration, more loop iterations
            size_t vl = __riscv_vsetvl_e32m1(w - x);
            vint32m1_t acc = __riscv_vmv_v_x_i32m1(0, vl);
            for (int ky = 0; ky < 5; ky++) {
                for (int kx = 0; kx < 5; kx++) {
                    const uint8_t* p = &padded[(y+ky)*pw+(x+kx)];
                    // mf4->mf2->m1 widening chain for LMUL=1 accumulator
                    vuint8mf4_t u8  = __riscv_vle8_v_u8mf4(p, vl);
                    vuint16mf2_t u16 = __riscv_vwcvtu_x_x_v_u16mf2(u8, vl);
                    vint16mf2_t s16 = __riscv_vreinterpret_v_u16mf2_i16mf2(u16);
                    vint32m1_t i32  = __riscv_vwcvt_x_x_v_i32m1(s16, vl);
                    acc = __riscv_vmacc_vx_i32m1(acc, krvv[ky][kx], i32, vl);
                }
            }
            vint32m1_t divided  = __riscv_vdiv_vx_i32m1(acc, 273, vl);
            vint32m1_t clamped  = __riscv_vmax_vx_i32m1(divided, 0, vl);
            vuint32m1_t uc      = __riscv_vreinterpret_v_i32m1_u32m1(clamped);
            vuint16mf2_t n16    = __riscv_vnclipu_wx_u16mf2(uc, 0, __RISCV_VXRM_RDN, vl);
            vuint8mf4_t n8      = __riscv_vnclipu_wx_u8mf4(n16, 0, __RISCV_VXRM_RDN, vl);
            __riscv_vse8_v_u8mf4(&out[y*w+x], n8, vl);
            x += vl;
        }
    }
    Image result(w, h);
    uint8_t* d = result.getData();
    for (int i = 0; i < w*h; i++) d[i] = out[i];
    delete[] padded;
    delete[] out;
    return result;
}

// LMUL=4 version: more elements per iteration, higher register pressure
// Faster than m2 at VLEN=256, similar at VLEN=512 due to register spilling
Image gaussianBlurRVV_m4(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();
    int pw = w + 4;
    int ph = h + 4;
    uint8_t* padded = new uint8_t[pw * ph]();
    const uint8_t* src = input.getConstData();
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            padded[(y+2)*pw+(x+2)] = src[y*w+x];
    uint8_t* out = new uint8_t[w * h];
    for (int y = 0; y < h; y++) {
        int x = 0;
        while (x < w) {
            // LMUL=4: double elements vs m2, but only 8 logical registers available
            size_t vl = __riscv_vsetvl_e32m4(w - x);
            vint32m4_t acc = __riscv_vmv_v_x_i32m4(0, vl);
            for (int ky = 0; ky < 5; ky++) {
                for (int kx = 0; kx < 5; kx++) {
                    const uint8_t* p = &padded[(y+ky)*pw+(x+kx)];
                    // m1->m2->m4 widening chain for LMUL=4 accumulator
                    vuint8m1_t u8   = __riscv_vle8_v_u8m1(p, vl);
                    vuint16m2_t u16 = __riscv_vwcvtu_x_x_v_u16m2(u8, vl);
                    vint16m2_t s16  = __riscv_vreinterpret_v_u16m2_i16m2(u16);
                    vint32m4_t i32  = __riscv_vwcvt_x_x_v_i32m4(s16, vl);
                    acc = __riscv_vmacc_vx_i32m4(acc, krvv[ky][kx], i32, vl);
                }
            }
            vint32m4_t divided  = __riscv_vdiv_vx_i32m4(acc, 273, vl);
            vint32m4_t clamped  = __riscv_vmax_vx_i32m4(divided, 0, vl);
            vuint32m4_t uc      = __riscv_vreinterpret_v_i32m4_u32m4(clamped);
            vuint16m2_t n16     = __riscv_vnclipu_wx_u16m2(uc, 0, __RISCV_VXRM_RDN, vl);
            vuint8m1_t n8       = __riscv_vnclipu_wx_u8m1(n16, 0, __RISCV_VXRM_RDN, vl);
            __riscv_vse8_v_u8m1(&out[y*w+x], n8, vl);
            x += vl;
        }
    }
    Image result(w, h);
    uint8_t* d = result.getData();
    for (int i = 0; i < w*h; i++) d[i] = out[i];
    delete[] padded;
    delete[] out;
    return result;
}

// Fixed-point version: replaces vdiv by 273 with vmul * 240 >> 16
// 65536/273 ≈ 240, so (sum * 240) >> 16 ≈ sum / 273
// Faster but slight precision loss (max error ~1 LSB)
Image gaussianBlurRVV_fp(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();
    int pw = w + 4;
    int ph = h + 4;
    uint8_t* padded = new uint8_t[pw * ph]();
    const uint8_t* src = input.getConstData();
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            padded[(y+2)*pw+(x+2)] = src[y*w+x];
    uint8_t* out = new uint8_t[w * h];
    for (int y = 0; y < h; y++) {
        int x = 0;
        while (x < w) {
            size_t vl = __riscv_vsetvl_e32m2(w - x);
            vint32m2_t acc = __riscv_vmv_v_x_i32m2(0, vl);
            for (int ky = 0; ky < 5; ky++) {
                for (int kx = 0; kx < 5; kx++) {
                    const uint8_t* p = &padded[(y+ky)*pw+(x+kx)];
                    vuint8mf2_t u8  = __riscv_vle8_v_u8mf2(p, vl);
                    vuint16m1_t u16 = __riscv_vwcvtu_x_x_v_u16m1(u8, vl);
                    vint16m1_t s16  = __riscv_vreinterpret_v_u16m1_i16m1(u16);
                    vint32m2_t i32  = __riscv_vwcvt_x_x_v_i32m2(s16, vl);
                    acc = __riscv_vmacc_vx_i32m2(acc, krvv[ky][kx], i32, vl);
                }
            }
            // Fixed-point: (sum * 240) >> 16 instead of sum / 273
            // 240/65536 = 0.003662 ≈ 1/273 = 0.003663 — error < 0.03%
            vint32m2_t scaled  = __riscv_vmul_vx_i32m2(acc, 240, vl);
            vint32m2_t divided = __riscv_vsra_vx_i32m2(scaled, 16, vl);
            vint32m2_t clamped = __riscv_vmax_vx_i32m2(divided, 0, vl);
            vuint32m2_t uc     = __riscv_vreinterpret_v_i32m2_u32m2(clamped);
            vuint16m1_t n16    = __riscv_vnclipu_wx_u16m1(uc, 0, __RISCV_VXRM_RDN, vl);
            vuint8mf2_t n8     = __riscv_vnclipu_wx_u8mf2(n16, 0, __RISCV_VXRM_RDN, vl);
            __riscv_vse8_v_u8mf2(&out[y*w+x], n8, vl);
            x += vl;
        }
    }
    Image result(w, h);
    uint8_t* d = result.getData();
    for (int i = 0; i < w*h; i++) d[i] = out[i];
    delete[] padded;
    delete[] out;
    return result;
}
