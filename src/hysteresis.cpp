#include "hysteresis.hpp"

Image hysteresisThreshold(const Image& input,
                          uint8_t lowThresh,
                          uint8_t highThresh) {
    int w = input.getWidth();
    int h = input.getHeight();
    Image output(w, h);

    // أول باسة: نصنف الـ pixels
    // 255 = strong edge, 128 = weak edge, 0 = مش edge
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            uint8_t val = input.getPixel(x, y);
            if (val >= highThresh)
                output.setPixel(x, y, 255);  // strong edge
            else if (val >= lowThresh)
                output.setPixel(x, y, 128);  // weak edge
            else
                output.setPixel(x, y, 0);    // مش edge
        }
    }

    // تاني باسة: الـ weak edges اللي جنب strong edge تبقى edges
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            if (output.getPixel(x, y) == 128) {
                // نشوف لو فيه strong edge في الـ 8 جيران
                bool hasStrong = false;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (output.getPixel(x+dx, y+dy) == 255) {
                            hasStrong = true;
                        }
                    }
                }
                if (hasStrong)
                    output.setPixel(x, y, 255);
                else
                    output.setPixel(x, y, 0);
            }
        }
    }

    return output;
}