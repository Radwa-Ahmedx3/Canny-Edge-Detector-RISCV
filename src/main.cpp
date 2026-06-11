#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
#include <iostream>
#include <time.h>

double get_ms() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

int main() {
    int width = 128;
    int height = 128;

    Image input(width, height);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            input.setPixel(x, y, (x + y) % 256);

    double t1, t2;

    // Gaussian Blur
    t1 = get_ms();
    for (int i = 0; i < 100; i++) gaussianBlur(input);
    Image blurred = gaussianBlur(input);
    t2 = get_ms();
    std::cout << "Gaussian: " << (t2 - t1) / 100.0 << " ms" << std::endl;

    // Sobel
    t1 = get_ms();
    for (int i = 0; i < 100; i++) sobelGradient(blurred);
    SobelResult result = sobelGradient(blurred);
    t2 = get_ms();
    std::cout << "Sobel: " << (t2 - t1) / 100.0 << " ms" << std::endl;

    // NMS
    t1 = get_ms();
    for (int i = 0; i < 100; i++) nonMaxSuppression(result.magnitude, result.direction);
    Image thinned = nonMaxSuppression(result.magnitude, result.direction);
    t2 = get_ms();
    std::cout << "NMS: " << (t2 - t1) / 100.0 << " ms" << std::endl;

    // Hysteresis
    t1 = get_ms();
    for (int i = 0; i < 100; i++) hysteresisThreshold(thinned, 50, 150);
    Image edges = hysteresisThreshold(thinned, 50, 150);
    t2 = get_ms();
    std::cout << "Hysteresis: " << (t2 - t1) / 100.0 << " ms" << std::endl;

    thinned.save("./nms_output.raw");
    edges.save("./final_edges.raw");

    return 0;
}
