#include "sobel.hpp"
#include <riscv_vector.h>
#include <cstdint>
#include <cstdlib>
static const int KX[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
static const int KY[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};
SobelResult sobelGradientRVV(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();
    const uint8_t* src = input.getConstData();
    int* gx_buf = new int[w * h]();
    int* gy_buf = new int[w * h]();
    int* mag    = new int[w * h]();
    Image magnitude(w, h);
    Image direction(w, h);
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
    int maxMag = 1;
    int n = w * h;
    int i = 0;
    while (i < n) {
        size_t vl = __riscv_vsetvl_e32m2(n - i);
        vint32m2_t vgx     = __riscv_vle32_v_i32m2(&gx_buf[i], vl);
        vint32m2_t vgy     = __riscv_vle32_v_i32m2(&gy_buf[i], vl);
        vint32m2_t vax     = __riscv_vmax_vv_i32m2(vgx, __riscv_vneg_v_i32m2(vgx, vl), vl);
        vint32m2_t vay     = __riscv_vmax_vv_i32m2(vgy, __riscv_vneg_v_i32m2(vgy, vl), vl);
        vint32m2_t vm      = __riscv_vadd_vv_i32m2(vax, vay, vl);
        __riscv_vse32_v_i32m2(&mag[i], vm, vl);
        vint32m1_t vinit   = __riscv_vmv_v_x_i32m1(maxMag, 1);
        vint32m1_t vmax_r  = __riscv_vredmax_vs_i32m2_i32m1(vm, vinit, vl);
        int cur = __riscv_vmv_x_s_i32m1_i32(vmax_r);
        if (cur > maxMag) maxMag = cur;
        i += vl;
    }
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
