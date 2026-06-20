#include "image.hpp"
#include "sobel.hpp"
#include <iostream>
#include <sys/time.h>
#include <cstring>
#include <iomanip>

SobelResult sobelGradient(const Image& input);
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
    std::cout << "\n=== PHASE 6.5: SOBEL MAGNITUDE (FULL VECTORIZATION) ===" << std::endl;
    
    double t0 = get_time_ms();
    for (int i = 0; i < RUNS; i++) {
        SobelResult result = sobelGradient(input);
    }
    double t1 = get_time_ms();
    double scalar_time = (t1 - t0) / RUNS;
    
    t0 = get_time_ms();
    for (int i = 0; i < RUNS; i++) {
        SobelResult result = sobelGradientRVV(input);
    }
    t1 = get_time_ms();
    double rvv_time = (t1 - t0) / RUNS;
    
    std::cout << "Scalar: " << scalar_time << " ms (baseline)" << std::endl;
    std::cout << "RVV:    " << rvv_time << " ms (" 
              << (100*(scalar_time-rvv_time)/scalar_time) << "% faster)" << std::endl;
    std::cout << "\nVector Operations:" << std::endl;
    std::cout << "  - Load Gx, Gy as i32 vectors" << std::endl;
    std::cout << "  - Compute absolute value (max of value and negation)" << std::endl;
    std::cout << "  - L1 norm: |Gx| + |Gy|" << std::endl;
    std::cout << "\nVector Reduction: vredmax" << std::endl;
    std::cout << "  - Collapse vector to scalar (find global max magnitude)" << std::endl;
    std::cout << "  - Extract result with vmv_x_s" << std::endl;
    std::cout << "  - Normalize to [0, 255]" << std::endl;

    return 0;
}
