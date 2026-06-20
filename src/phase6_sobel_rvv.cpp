#include "image.hpp"
#include "sobel.hpp"
#include <iostream>
#include <sys/time.h>
#include <cstring>
#include <iomanip>

SobelResult sobelGradientRVV(const Image& input);

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
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n=== PHASE 6.3: SOBEL MAGNITUDE (L1 NORM) ===" << std::endl;
    
    // Scalar Sobel
    double t0 = get_time_ms();
    for (int i = 0; i < RUNS; i++) {
        SobelResult result = sobelGradient(input);
    }
    double t1 = get_time_ms();
    double scalar_time = (t1 - t0) / RUNS;
    
    // RVV Sobel (with vector reductions)
    t0 = get_time_ms();
    for (int i = 0; i < RUNS; i++) {
        SobelResult result = sobelGradientRVV(input);
    }
    t1 = get_time_ms();
    double rvv_time = (t1 - t0) / RUNS;
    
    std::cout << "Scalar: " << scalar_time << " ms" << std::endl;
    std::cout << "RVV:    " << rvv_time << " ms";
    if (rvv_time < scalar_time) {
        std::cout << " (" << (100*(scalar_time-rvv_time)/scalar_time) << "% faster)";
    } else {
        std::cout << " (" << (100*(rvv_time-scalar_time)/scalar_time) << "% slower)";
    }
    std::cout << std::endl;
    std::cout << "\nVector Reductions: vredmax (find max magnitude)" << std::endl;
    std::cout << "Scalar Extract: vmv_x_s (result to scalar)" << std::endl;

    return 0;
}
