#include "gaussian.hpp"
#include <cstdint>

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
            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    int px = x + kx;
                    int py = y + ky;
                    if (px >= 0 && px < w && py >= 0 && py < h) {
                        sum += input.getPixel(px, py) * kernel[ky+2][kx+2];
                    }
                }
            }
            output.setPixel(x, y, (uint8_t)(sum / 273));
        }
    }
    return output;
}

// Padding experiment: pre-pad image with zeros to avoid boundary checks
// This allows the compiler to auto-vectorize the inner loop
Image gaussianBlurPadded(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();

    int pw = w + 4;
    int ph = h + 4;
    Image padded(pw, ph);

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            padded.setPixel(x + 2, y + 2, input.getPixel(x, y));

    Image output(w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum = 0;
            for (int ky = 0; ky < 5; ky++) {
                for (int kx = 0; kx < 5; kx++) {
                    sum += padded.getData()[(y+ky) * pw + (x+kx)]
                           * kernel[ky][kx];
                }
            }
            output.setPixel(x, y, (uint8_t)(sum / 273));
        }
    }
    return output;
}