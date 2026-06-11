#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
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
    std::cout << "Gaussian Blur done!" << std::endl;

    // Sobel Gradient
    SobelResult result = sobelGradient(blurred);
    std::cout << "Sobel done!" << std::endl;

    // Non-Maximum Suppression
    Image thinned = nonMaxSuppression(result.magnitude, result.direction);
    std::cout << "NMS done!" << std::endl;
    thinned.save("nms_output.raw");
    std::cout << "NMS output saved!" << std::endl;

    // Hysteresis Thresholding
    Image edges = hysteresisThreshold(thinned, 50, 150);
    std::cout << "Hysteresis done!" << std::endl;
    edges.save("final_edges.raw");
    std::cout << "Final edges saved!" << std::endl;
    return 0;
}
