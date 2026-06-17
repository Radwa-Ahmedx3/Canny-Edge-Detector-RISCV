#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include <iostream>
#include <sys/time.h>

double get_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

int main() {
    int w = 128, h = 128;
    const int RUNS = 100;
    Image input(w, h);
    if (!input.load("test_real.raw")) {
        std::cout << "Failed to load!" << std::endl;
        return 1;
    }
    Image blurred = gaussianBlur(input);
    double l1_total = 0, l2_total = 0;
    SobelResult l1 = sobelGradient(blurred);
    SobelResult l2 = sobelGradientL2(blurred);
    for (int i = 0; i < RUNS; i++) {
        double t1 = get_ms(); sobelGradient(blurred); l1_total += get_ms() - t1;
        double t2 = get_ms(); sobelGradientL2(blurred); l2_total += get_ms() - t2;
    }
    l1.magnitude.save("l1_magnitude.raw");
    l2.magnitude.save("l2_magnitude.raw");
    int maxDiff = 0; double totalDiff = 0;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int diff = abs((int)l1.magnitude.getPixel(x,y) - (int)l2.magnitude.getPixel(x,y));
            if (diff > maxDiff) maxDiff = diff;
            totalDiff += diff;
        }
    std::cout << "=== L1 vs L2 Comparison ===" << std::endl;
    std::cout << "L1 time: " << l1_total/RUNS << " ms" << std::endl;
    std::cout << "L2 time: " << l2_total/RUNS << " ms" << std::endl;
    std::cout << "L2 overhead: " << (l2_total/l1_total-1)*100 << "% slower" << std::endl;
    std::cout << "Max pixel diff: " << maxDiff << std::endl;
    std::cout << "Avg pixel diff: " << totalDiff/(w*h) << std::endl;
    return 0;
}
