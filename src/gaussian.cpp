#include "gaussian.hpp"
#include <cstdint>

// الـ 5x5 Gaussian kernel - مجموع الأرقام = 273
// بنستخدمه عشان نعمل blur للصورة ونقلل الـ noise
static const int kernel[5][5] = {
    {1,  4,  7,  4,  1},
    {4, 16, 26, 16,  4},
    {7, 26, 41, 26,  7},
    {4, 16, 26, 16,  4},
    {1,  4,  7,  4,  1}
};

Image gaussianBlur(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();
    Image output(w, h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum = 0;

            // نطبق الـ kernel على كل pixel
            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    // zero-padding: لو خارج الصورة نعامله كـ 0
                    int px = x + kx;
                    int py = y + ky;
                    if (px >= 0 && px < w && py >= 0 && py < h) {
                        sum += input.getPixel(px, py) * kernel[ky+2][kx+2];
                    }
                }
            }

            // نقسم على 273 ونحط النتيجة في الـ output
            output.setPixel(x, y, (uint8_t)(sum / 273));
        }
    }

    return output;
}