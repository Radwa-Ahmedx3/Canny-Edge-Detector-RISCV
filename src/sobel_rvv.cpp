#include "sobel_rvv.hpp"
#include <riscv_vector.h>
#include <cstdint>
#include <cstdlib>

static int16_t mag_buf[128 * 128];
static int16_t gx_buf16[128 * 128];
static int16_t gy_buf16[128 * 128];

SobelResult sobelGradientRVV(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();
    Image magnitude(w, h);
    Image direction(w, h);

    static const int Kx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    static const int Ky[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};

    // Pass 1: Compute Gx, Gy, magnitude
    for (int y = 0; y < h; y++) {
        const uint8_t* ra = input.getData() + (y-1)*w;
        const uint8_t* rm = input.getData() + y*w;
        const uint8_t* rb = input.getData() + (y+1)*w;

        // Border rows: scalar
        if (y == 0 || y == h-1) {
            for (int x = 0; x < w; x++) {
                int gx = 0, gy = 0;
                for (int ky=-1; ky<=1; ky++)
                    for (int kx=-1; kx<=1; kx++) {
                        int px=x+kx, py=y+ky;
                        if (px>=0&&px<w&&py>=0&&py<h) {
                            int p = input.getPixel(px, py);
                            gx += p * Kx[ky+1][kx+1];
                            gy += p * Ky[ky+1][kx+1];
                        }
                    }
                gx_buf16[y*w+x] = (int16_t)gx;
                gy_buf16[y*w+x] = (int16_t)gy;
                mag_buf[y*w+x]  = (int16_t)(abs(gx)+abs(gy));
            }
            continue;
        }

        // Border columns scalar
        gx_buf16[y*w+0] = 0; gy_buf16[y*w+0] = 0; mag_buf[y*w+0] = 0;
        gx_buf16[y*w+w-1] = 0; gy_buf16[y*w+w-1] = 0; mag_buf[y*w+w-1] = 0;

        // Interior columns: vectorized
        int x = 1;
        while (x <= w-2) {
            size_t vl = __riscv_vsetvl_e8m2(w-1-x);

            // Load 8 shifted pixel vectors
            vuint8m2_t ra_l = __riscv_vle8_v_u8m2(ra+x-1, vl);
            vuint8m2_t ra_c = __riscv_vle8_v_u8m2(ra+x,   vl);
            vuint8m2_t ra_r = __riscv_vle8_v_u8m2(ra+x+1, vl);
            vuint8m2_t rm_l = __riscv_vle8_v_u8m2(rm+x-1, vl);
            vuint8m2_t rm_r = __riscv_vle8_v_u8m2(rm+x+1, vl);
            vuint8m2_t rb_l = __riscv_vle8_v_u8m2(rb+x-1, vl);
            vuint8m2_t rb_c = __riscv_vle8_v_u8m2(rb+x,   vl);
            vuint8m2_t rb_r = __riscv_vle8_v_u8m2(rb+x+1, vl);

            // Widen u8m2 -> i16m4
            vint16m4_t ral = __riscv_vreinterpret_v_u16m4_i16m4(__riscv_vwcvtu_x_x_v_u16m4(ra_l, vl));
            vint16m4_t rac = __riscv_vreinterpret_v_u16m4_i16m4(__riscv_vwcvtu_x_x_v_u16m4(ra_c, vl));
            vint16m4_t rar = __riscv_vreinterpret_v_u16m4_i16m4(__riscv_vwcvtu_x_x_v_u16m4(ra_r, vl));
            vint16m4_t rml = __riscv_vreinterpret_v_u16m4_i16m4(__riscv_vwcvtu_x_x_v_u16m4(rm_l, vl));
            vint16m4_t rmr = __riscv_vreinterpret_v_u16m4_i16m4(__riscv_vwcvtu_x_x_v_u16m4(rm_r, vl));
            vint16m4_t rbl = __riscv_vreinterpret_v_u16m4_i16m4(__riscv_vwcvtu_x_x_v_u16m4(rb_l, vl));
            vint16m4_t rbc = __riscv_vreinterpret_v_u16m4_i16m4(__riscv_vwcvtu_x_x_v_u16m4(rb_c, vl));
            vint16m4_t rbr = __riscv_vreinterpret_v_u16m4_i16m4(__riscv_vwcvtu_x_x_v_u16m4(rb_r, vl));

            // Gx = (ra_r - ra_l) + 2*(rm_r - rm_l) + (rb_r - rb_l)
            vint16m4_t gx = __riscv_vsub_vv_i16m4(rar, ral, vl);
            vint16m4_t mid = __riscv_vsub_vv_i16m4(rmr, rml, vl);
            gx = __riscv_vadd_vv_i16m4(gx, __riscv_vsll_vx_i16m4(mid, 1, vl), vl);
            gx = __riscv_vadd_vv_i16m4(gx, __riscv_vsub_vv_i16m4(rbr, rbl, vl), vl);

            // Gy = -(ra_l + 2*ra_c + ra_r) + (rb_l + 2*rb_c + rb_r)
            vint16m4_t top = __riscv_vadd_vv_i16m4(ral, __riscv_vadd_vv_i16m4(
                __riscv_vsll_vx_i16m4(rac, 1, vl), rar, vl), vl);
            vint16m4_t bot = __riscv_vadd_vv_i16m4(rbl, __riscv_vadd_vv_i16m4(
                __riscv_vsll_vx_i16m4(rbc, 1, vl), rbr, vl), vl);
            vint16m4_t gy = __riscv_vsub_vv_i16m4(bot, top, vl);

            // Store Gx, Gy
            __riscv_vse16_v_i16m4(gx_buf16 + y*w + x, gx, vl);
            __riscv_vse16_v_i16m4(gy_buf16 + y*w + x, gy, vl);

            // |Gx| + |Gy|
            vint16m4_t abs_gx = __riscv_vmax_vv_i16m4(gx,
                __riscv_vneg_v_i16m4(gx, vl), vl);
            vint16m4_t abs_gy = __riscv_vmax_vv_i16m4(gy,
                __riscv_vneg_v_i16m4(gy, vl), vl);
            vint16m4_t vmag = __riscv_vadd_vv_i16m4(abs_gx, abs_gy, vl);
            __riscv_vse16_v_i16m4(mag_buf + y*w + x, vmag, vl);

            x += vl;
        }
    }

    // Find max using vector reduction
    int maxMag = 1;
    int n = w * h;
    int i = 0;
    vint16m1_t vmax_val = __riscv_vmv_v_x_i16m1(0, 1);
    while (i < n) {
        size_t vl = __riscv_vsetvl_e16m4(n - i);
        vint16m4_t vmag = __riscv_vle16_v_i16m4(mag_buf + i, vl);
        vmax_val = __riscv_vredmax_vs_i16m4_i16m1(vmag, vmax_val, vl);
        i += vl;
    }
    maxMag = __riscv_vmv_x_s_i16m1_i16(vmax_val);
    if (maxMag < 1) maxMag = 1;

    // Normalize and direction (scalar)
    for (int idx = 0; idx < n; idx++) {
        magnitude.setPixel(idx%w, idx/w,
            (uint8_t)((int)mag_buf[idx] * 255 / maxMag));
        int gx = gx_buf16[idx], gy = gy_buf16[idx];
        int ax = abs(gx), ay = abs(gy);
        uint8_t dir;
        if (ay*5 < ax*2)       dir = 0;
        else if (ay*5 > ax*12) dir = 2;
        else if (gx*gy > 0)    dir = 1;
        else                    dir = 3;
        direction.setPixel(idx%w, idx/w, dir);
    }

    SobelResult result = {magnitude, direction};
    return result;
}
