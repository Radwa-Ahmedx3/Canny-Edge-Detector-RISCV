#include "gaussian.hpp"

static const int kernel[5][5] = {
    {1,  4,  7,  4,  1},
    {4, 16, 26, 16,  4},
    {7, 26, 41, 26,  7},
    {4, 16, 26, 16,  4},
    {1,  4,  7,  4,  1}
};

Image gaussianBlurRVV(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();
    Image output(w, h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum = 0;
            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    int px = x + kx;
                    int py = y + ky;
                    if (px >= 0 && px < w && py >= 0 && py < h) {
                        sum += input.getPixel(px, py) * kernel[ky+2][kx+2];
                    }
                }
            }
            // Different rounding: use +136 instead of /2 to show ±1 difference
            output.setPixel(x, y, (uint8_t)((sum + 136) / 273));
        }
    }
    return output;
}
