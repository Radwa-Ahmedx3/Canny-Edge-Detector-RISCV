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

    const int RUNS = 100;
    double g_total = 0, s_total = 0, n_total = 0, h_total = 0;

    for (int i = 0; i < RUNS; i++) {
        double t0 = get_ms();
        Image blurred = gaussianBlur(input);
        double t1 = get_ms();
        SobelResult result = sobelGradient(blurred);
        double t2 = get_ms();
        Image thinned = nonMaxSuppression(result.magnitude, result.direction);
        double t3 = get_ms();
        Image edges = hysteresisThreshold(thinned, 50, 150);
        double t4 = get_ms();

        g_total += t1 - t0;
        s_total += t2 - t1;
        n_total += t3 - t2;
        h_total += t4 - t3;

        if (i == RUNS - 1) {
            thinned.save("./nms_output.raw");
            edges.save("./final_edges.raw");
        }
    }

    double avg_g = g_total / RUNS;
    double avg_s = s_total / RUNS;
    double avg_n = n_total / RUNS;
    double avg_h = h_total / RUNS;
    double avg_total = avg_g + avg_s + avg_n + avg_h;

    std::cout << "Averaged over " << RUNS << " runs:\n";
    std::cout << "-------------------------\n";
    std::cout << "Gaussian:   " << avg_g << " ms  ("
              << (avg_g / avg_total * 100) << "%)\n";
    std::cout << "Sobel:      " << avg_s << " ms  ("
              << (avg_s / avg_total * 100) << "%)\n";
    std::cout << "NMS:        " << avg_n << " ms  ("
              << (avg_n / avg_total * 100) << "%)\n";
    std::cout << "Hysteresis: " << avg_h << " ms  ("
              << (avg_h / avg_total * 100) << "%)\n";
    std::cout << "-------------------------\n";
    std::cout << "Total:      " << avg_total << " ms\n";

    return 0;
}