#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
#include <iostream>
#include <sys/time.h>

double get_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

int main() {
    int width = 128, height = 128;

    // Generate synthetic square image in memory
    Image input(width, height);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            bool inSquare = (x>=30 && x<90 && y>=30 && y<90);
            input.setPixel(x, y, inSquare ? 255 : 0);
        }

    double t1, t2;

    t1 = get_ms();
    for (int i = 0; i < 100; i++) gaussianBlur(input);
    Image blurred = gaussianBlur(input);
    t2 = get_ms();
    std::cout << "Gaussian: " << (t2-t1)/100.0 << " ms" << std::endl;

    t1 = get_ms();
    for (int i = 0; i < 100; i++) sobelGradient(blurred);
    SobelResult result = sobelGradient(blurred);
    t2 = get_ms();
    std::cout << "Sobel: " << (t2-t1)/100.0 << " ms" << std::endl;

    t1 = get_ms();
    for (int i = 0; i < 100; i++) nonMaxSuppression(result.magnitude, result.direction);
    Image thinned = nonMaxSuppression(result.magnitude, result.direction);
    t2 = get_ms();
    std::cout << "NMS: " << (t2-t1)/100.0 << " ms" << std::endl;

    t1 = get_ms();
    for (int i = 0; i < 100; i++) hysteresisThreshold(thinned, 20, 80);
    Image edges = hysteresisThreshold(thinned, 20, 80);
    t2 = get_ms();
    std::cout << "Hysteresis: " << (t2-t1)/100.0 << " ms" << std::endl;

    return 0;
}
