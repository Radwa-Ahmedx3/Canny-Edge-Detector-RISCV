#include "sobel.hpp"
#include <riscv_vector.h>
#include <cstdint>
#include <cstdlib>

static const int KX[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
static const int KY[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};

// sobelGradientRVV: RVV-accelerated L1 magnitude computation
// Strategy: scalar loop computes Gx/Gy buffers (boundary handling kept scalar)
//           RVV loop computes |Gx|+|Gy| magnitude and finds global max
SobelResult sobelGradientRVV(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();
    const uint8_t* src = input.getConstData();
    int* gx_buf = new int[w * h]();
    int* gy_buf = new int[w * h]();
    int* mag    = new int[w * h]();
    Image magnitude(w, h);
    Image direction(w, h);

    // Scalar: compute Gx and Gy with boundary handling
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int gx = 0, gy = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int px = x + kx, py = y + ky;
                    int p = (px>=0&&px<w&&py>=0&&py<h) ? src[py*w+px] : 0;
                    gx += p * KX[ky+1][kx+1];
                    gy += p * KY[ky+1][kx+1];
                }
            }
            gx_buf[y*w+x] = gx;
            gy_buf[y*w+x] = gy;
        }
    }

    // RVV: compute magnitude = |Gx| + |Gy| and find global max
    int maxMag = 1;
    int n = w * h;
    int i = 0;
    while (i < n) {
        // (1) Set vector length for 32-bit elements LMUL=2
        // (2) LMUL=2 balances throughput vs register pressure for this kernel
        // (3) vl scales with VLEN: larger VLEN = more elements per iteration
        size_t vl = __riscv_vsetvl_e32m2(n - i);

        // (1) Load vl Gx values as int32 vector
        // (2) LMUL=2 matches accumulator size — no widening needed (already int32)
        // (3) At any VLEN, loads exactly vl consecutive int32 values
        vint32m2_t vgx = __riscv_vle32_v_i32m2(&gx_buf[i], vl);

        // (1) Load vl Gy values as int32 vector
        // (2) Same LMUL=2 as vgx for element-wise add later
        // (3) Same VLA behavior — vl elements regardless of VLEN
        vint32m2_t vgy = __riscv_vle32_v_i32m2(&gy_buf[i], vl);

        // (1) Compute abs(Gx): negate vgx then take element-wise max(vgx, -vgx)
        // (2) No dedicated vabs for int32 in RVV — vneg+vmax is the standard idiom
        // (3) Works identically at any VLEN — purely element-wise operation
        vint32m2_t vax = __riscv_vmax_vv_i32m2(vgx, __riscv_vneg_v_i32m2(vgx, vl), vl);

        // (1) Compute abs(Gy): same idiom as abs(Gx)
        // (2) Same LMUL=2 to match vax for addition
        // (3) Element-wise — no VLEN dependency
        vint32m2_t vay = __riscv_vmax_vv_i32m2(vgy, __riscv_vneg_v_i32m2(vgy, vl), vl);

        // (1) L1 magnitude: |Gx| + |Gy| element-wise
        // (2) LMUL=2 — same as inputs, no widening needed
        // (3) vl additions in parallel regardless of VLEN
        vint32m2_t vm = __riscv_vadd_vv_i32m2(vax, vay, vl);

        // (1) Store vl magnitude values to memory
        // (2) LMUL=2 matches vm type
        // (3) Stores exactly vl elements — VLA strip-mining pattern
        __riscv_vse32_v_i32m2(&mag[i], vm, vl);

        // (1) Initialize scalar vector with current maxMag (size=1, m1)
        // (2) LMUL=1 required by vredmax destination type — reduction outputs m1
        // (3) Always size 1 regardless of VLEN — scalar seed for reduction
        vint32m1_t vinit = __riscv_vmv_v_x_i32m1(maxMag, 1);

        // (1) Vector reduction: find max of all vl elements in vm, seed with vinit
        // (2) Input is m2, output is m1 — reduction always produces m1 result
        // (3) Collapses vl elements to 1 scalar in element 0, same at any VLEN
        vint32m1_t vmax_r = __riscv_vredmax_vs_i32m2_i32m1(vm, vinit, vl);

        // (1) Extract scalar from element 0 of reduction result
        // (2) vmv_x_s always extracts from m1 register element 0
        // (3) Always returns 1 scalar value regardless of VLEN
        int cur = __riscv_vmv_x_s_i32m1_i32(vmax_r);
        if (cur > maxMag) maxMag = cur;

        i += vl;
    }

    // Scalar: normalize magnitude and compute direction
    for (int j = 0; j < n; j++) {
        magnitude.setPixel(j%w, j/w, (uint8_t)(mag[j]*255/maxMag));
        int gx = gx_buf[j], gy = gy_buf[j];
        int ax = abs(gx), ay = abs(gy);
        uint8_t dir;
        if (ay*5 < ax*2)       dir = 0;
        else if (ay*5 > ax*12) dir = 2;
        else if (gx*gy > 0)    dir = 1;
        else                    dir = 3;
        direction.setPixel(j%w, j/w, dir);
    }

    delete[] gx_buf;
    delete[] gy_buf;
    delete[] mag;
    SobelResult result = {magnitude, direction};
    return result;
}
