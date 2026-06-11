#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include <iostream>

int main() {
    int width = 128;
    int height = 128;

    // عمل صورة تجريبية
    Image input(width, height);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            input.setPixel(x, y, (x + y) % 256);

    // Gaussian Blur
    Image blurred = gaussianBlur(input);
    blurred.save("blurred.raw");
    std::cout << "Gaussian Blur done!" << std::endl;

    // Sobel Gradient
    SobelResult result = sobelGradient(blurred);
    result.magnitude.save("magnitude.raw");
    std::cout << "Sobel done!" << std::endl;

    return 0;
}