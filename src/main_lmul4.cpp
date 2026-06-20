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
    Image input(128, 128);
    if (!input.load("./test_input.raw")) {
        std::cerr << "Failed to load image\n";
        return 1;
    }

    const int RUNS = 100;
    double g_total = 0, s_total = 0, n_total = 0, h_total = 0;

    for (int i = 0; i < RUNS; i++) {
        double t0 = get_ms();
        Image blurred = gaussianBlurRVV_m4(input);
        double t1 = get_ms();
        SobelResult result = sobelGradientRVV(blurred);
        double t2 = get_ms();
        Image thinned = nonMaxSuppression(result.magnitude, result.direction);
        double t3 = get_ms();
        Image edges = hysteresisThreshold(thinned, 20, 60);
        double t4 = get_ms();

        g_total += t1 - t0;
        s_total += t2 - t1;
        n_total += t3 - t2;
        h_total += t4 - t3;

        if (i == RUNS - 1) {
            blurred.save("./blurred.raw");
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
    std::cout << "Gaussian:   " << avg_g << " ms  (" << (avg_g/avg_total*100) << "%)\n";
    std::cout << "Sobel:      " << avg_s << " ms  (" << (avg_s/avg_total*100) << "%)\n";
    std::cout << "NMS:        " << avg_n << " ms  (" << (avg_n/avg_total*100) << "%)\n";
    std::cout << "Hysteresis: " << avg_h << " ms  (" << (avg_h/avg_total*100) << "%)\n";
    std::cout << "-------------------------\n";
    std::cout << "Total:      " << avg_total << " ms\n";

    return 0;
}
