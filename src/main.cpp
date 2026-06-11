#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include <iostream>

int main() {
    int width = 128;
    int height = 128;

    Image input(width, height);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            input.setPixel(x, y, (x + y) % 256);

    Image blurred = gaussianBlur(input);
    blurred.save("/tmp/blurred.raw");
    std::cout << "Gaussian Blur done!" << std::endl;

    SobelResult result = sobelGradient(blurred);
    result.magnitude.save("/tmp/magnitude.raw");
    std::cout << "Sobel done!" << std::endl;

    return 0;
}
