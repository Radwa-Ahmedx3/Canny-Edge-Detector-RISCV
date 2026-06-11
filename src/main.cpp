#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include <iostream>

int main() {
    int width = 128;
    int height = 128;

    // مربع أبيض على خلفية سوداء
    Image input(width, height);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            if (x > 40 && x < 88 && y > 40 && y < 88)
                input.setPixel(x, y, 255);
            else
                input.setPixel(x, y, 0);
        }

    // Gaussian Blur
    Image blurred = gaussianBlur(input);
    blurred.save("/tmp/blurred.raw");
    std::cout << "Gaussian Blur done!" << std::endl;

    // Sobel Gradient
    SobelResult result = sobelGradient(blurred);
    result.magnitude.save("/tmp/magnitude.raw");
    std::cout << "Sobel done!" << std::endl;

    return 0;
}
