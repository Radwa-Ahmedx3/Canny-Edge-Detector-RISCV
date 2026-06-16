#include "gaussian_rvv.hpp"
#include <riscv_vector.h>
#include <cstdint>
#include <algorithm>

// 1D separable kernel: [1, 4, 7, 4, 1] (sum=17)
// Full 2D = outer product / 289 ≈ Gaussian
static const int16_t h_kernel[5] = {1, 4, 7, 4, 1};

// Intermediate buffer for horizontal pass results
static int32_t temp_buf[128 * 128];

Image gaussianBlurRVV(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();
    Image output(w, h);

    // === PASS 1: Horizontal 1D convolution ===
    // For each row, convolve with [1,4,7,4,1]
    // Output stored as int32 in temp_buf (no normalization yet)
    for (int y = 0; y < h; y++) {
        int32_t* temp_row = temp_buf + y * w;
        int x = 2;

        // Vectorized interior columns
        while (x < w - 2) {
            size_t vl = __riscv_vsetvl_e8m2(w - 2 - x);
            vint32m8_t acc = __riscv_vmv_v_x_i32m8(0, vl);

            for (int kx = -2; kx <= 2; kx++) {
                int16_t coeff = h_kernel[kx + 2];
                const uint8_t* src = input.getData() + y * w + (x + kx);
                vuint8m2_t pixels  = __riscv_vle8_v_u8m2(src, vl);
                vuint16m4_t pix16  = __riscv_vwcvtu_x_x_v_u16m4(pixels, vl);
                vuint32m8_t pix32  = __riscv_vwcvtu_x_x_v_u32m8(pix16, vl);
                vint32m8_t  spix   = __riscv_vreinterpret_v_u32m8_i32m8(pix32);
                acc = __riscv_vmacc_vx_i32m8(acc, coeff, spix, vl);
            }
            __riscv_vse32_v_i32m8(temp_row + x, acc, vl);
            x += vl;
        }

        // Scalar border columns
        for (int bx = 0; bx < w; bx++) {
            if (bx >= 2 && bx < w - 2) continue;
            int32_t sum = 0;
            for (int kx = -2; kx <= 2; kx++) {
                int px = bx + kx;
                if (px >= 0 && px < w)
                    sum += input.getPixel(px, y) * h_kernel[kx + 2];
            }
            temp_row[bx] = sum;
        }
    }

    // === PASS 2: Vertical 1D convolution + normalize ===
    // For each column, convolve temp_buf rows with [1,4,7,4,1]
    // then divide by 289 (17x17)
    // Fixed-point: (sum * 227) >> 16 ≈ sum / 289
    for (int y = 2; y < h - 2; y++) {
        uint8_t* out_row = output.getData() + y * w;
        int x = 2;

        while (x < w - 2) {
            size_t vl = __riscv_vsetvl_e32m4(w - 2 - x);
            vint32m4_t acc = __riscv_vmv_v_x_i32m4(0, vl);

            for (int ky = -2; ky <= 2; ky++) {
                int16_t coeff = h_kernel[ky + 2];
                const int32_t* src = temp_buf + (y + ky) * w + x;
                vint32m4_t vals = __riscv_vle32_v_i32m4(src, vl);
                acc = __riscv_vmacc_vx_i32m4(acc, coeff, vals, vl);
            }

            // Divide by 289: (sum * 227) >> 16
            acc = __riscv_vmul_vx_i32m4(acc, 227, vl);
            acc = __riscv_vsra_vx_i32m4(acc, 16, vl);

            // Clamp to [0, 255]
            acc = __riscv_vmax_vx_i32m4(acc, 0, vl);
            acc = __riscv_vmin_vx_i32m4(acc, 255, vl);

            // Narrow: i32m4 -> u16m2 -> u8m1
            vuint32m4_t uacc    = __riscv_vreinterpret_v_i32m4_u32m4(acc);
            vuint16m2_t narrow16 = __riscv_vnclipu_wx_u16m2(uacc, 0, __RISCV_VXRM_RNU, vl);
            vuint8m1_t  result   = __riscv_vnclipu_wx_u8m1(narrow16, 0, __RISCV_VXRM_RNU, vl);
            __riscv_vse8_v_u8m1(out_row + x, result, vl);
            x += vl;
        }
    }

    // Scalar border rows
    for (int by = 0; by < h; by++) {
        for (int bx = 0; bx < w; bx++) {
            if (by >= 2 && by < h-2 && bx >= 2 && bx < w-2) continue;
            int32_t sum = 0;
            for (int ky = -2; ky <= 2; ky++) {
                int py = by + ky;
                if (py >= 0 && py < h)
                    sum += temp_buf[py * w + bx] * h_kernel[ky + 2];
            }
            int val = (int)((sum * 227) >> 16);
            val = val < 0 ? 0 : (val > 255 ? 255 : val);
            output.setPixel(bx, by, (uint8_t)val);
        }
    }

    return output;
}
