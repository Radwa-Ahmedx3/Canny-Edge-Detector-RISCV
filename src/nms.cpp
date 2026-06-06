#include "nms.hpp"

Image nonMaxSuppression(const Image& magnitude, const Image& direction) {
    int w = magnitude.getWidth();
    int h = magnitude.getHeight();
    Image output(w, h);

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            uint8_t mag = magnitude.getPixel(x, y);
            uint8_t dir = direction.getPixel(x, y);

            uint8_t n1 = 0, n2 = 0;

            // نقارن كل pixel بجيرانه في اتجاه الـ gradient
            if (dir == 0) {        // أفقي
                n1 = magnitude.getPixel(x-1, y);
                n2 = magnitude.getPixel(x+1, y);
            } else if (dir == 1) { // 45 درجة
                n1 = magnitude.getPixel(x-1, y-1);
                n2 = magnitude.getPixel(x+1, y+1);
            } else if (dir == 2) { // رأسي
                n1 = magnitude.getPixel(x, y-1);
                n2 = magnitude.getPixel(x, y+1);
            } else {               // 135 درجة
                n1 = magnitude.getPixel(x+1, y-1);
                n2 = magnitude.getPixel(x-1, y+1);
            }

            // لو الـ pixel ده أكبر من جيرانه يبقى edge، غير كده يبقى 0
            if (mag >= n1 && mag >= n2)
                output.setPixel(x, y, mag);
            else
                output.setPixel(x, y, 0);
        }
    }

    return output;
}