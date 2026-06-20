#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
#include <iostream>
#include <sys/time.h>
#include <cstring>
#include <iomanip>

double get_time_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

int main() {
    Image input(128, 128);
    uint8_t* data = input.getData();
    memset(data, 0, 128*128);
    data[64*128 + 64] = 255;

    const int RUNS = 100;
    double gaussian_total = 0, sobel_total = 0, nms_total = 0, hysteresis_total = 0;

    for (int i = 0; i < RUNS; i++) {
        double t0 = get_time_ms();
        Image blurred = gaussianBlur(input);
        double t1 = get_time_ms();
        gaussian_total += (t1 - t0);

        SobelResult result = sobelGradient(blurred);
        double t2 = get_time_ms();
        sobel_total += (t2 - t1);

        Image thinned = nonMaxSuppression(result.magnitude, result.direction);
        double t3 = get_time_ms();
        nms_total += (t3 - t2);

        Image edges = hysteresisThreshold(thinned, 20, 60);
        double t4 = get_time_ms();
        hysteresis_total += (t4 - t3);
    }

    double avg_gaussian = gaussian_total / RUNS;
    double avg_sobel = sobel_total / RUNS;
    double avg_nms = nms_total / RUNS;
    double avg_hysteresis = hysteresis_total / RUNS;
    double total = avg_gaussian + avg_sobel + avg_nms + avg_hysteresis;

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "\n=== PHASE 5 PROFILING ===" << std::endl;
    std::cout << "Gaussian:   " << std::setw(6) << avg_gaussian << " ms (" 
              << std::setw(5) << (avg_gaussian/total*100) << "%)" << std::endl;
    std::cout << "Sobel:      " << std::setw(6) << avg_sobel << " ms (" 
              << std::setw(5) << (avg_sobel/total*100) << "%)" << std::endl;
    std::cout << "NMS:        " << std::setw(6) << avg_nms << " ms (" 
              << std::setw(5) << (avg_nms/total*100) << "%)" << std::endl;
    std::cout << "Hysteresis: " << std::setw(6) << avg_hysteresis << " ms (" 
              << std::setw(5) << (avg_hysteresis/total*100) << "%)" << std::endl;
    std::cout << "─────────────────────────" << std::endl;
    std::cout << "Total:      " << std::setw(6) << total << " ms (100.0%)" << std::endl;

    return 0;
}
