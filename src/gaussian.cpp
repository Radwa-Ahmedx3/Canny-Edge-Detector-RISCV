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

// Padded version: pre-pads image with zeros to eliminate boundary checks
// Created to test if compiler can auto-vectorize without boundary conditions
// Result: still not vectorized — getPixel() treated as exception-throwing by compiler
Image gaussianBlurPadded(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();

    // Create padded image (add 2 pixels on each side)
    int pw = w + 4, ph = h + 4;
    Image padded(pw, ph);

    // Copy input into center, borders remain zero (zero-padding)
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            padded.setPixel(x + 2, y + 2, input.getPixel(x, y));

    Image output(w, h);
    static const int kernel[5][5] = {
        {1,  4,  7,  4,  1},
        {4, 16, 26, 16,  4},
        {7, 26, 41, 26,  7},
        {4, 16, 26, 16,  4},
        {1,  4,  7,  4,  1}
    };

    // No boundary check needed — all accesses are within padded image
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum = 0;
            for (int ky = 0; ky < 5; ky++)
                for (int kx = 0; kx < 5; kx++)
                    sum += padded.getPixel(x + kx, y + ky) * kernel[ky][kx];
            output.setPixel(x, y, (uint8_t)(sum / 273));
        }
    }
    return output;
}
