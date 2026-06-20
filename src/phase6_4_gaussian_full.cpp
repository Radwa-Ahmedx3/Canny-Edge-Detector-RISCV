#include "image.hpp"
#include <iostream>
#include <sys/time.h>
#include <cstring>
#include <iomanip>

Image gaussianBlur(const Image& input);
Image gaussianBlurRVV(const Image& input);
Image gaussianBlurRVV_m1(const Image& input);
Image gaussianBlurRVV_m4(const Image& input);

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
    std::cout << "\n=== PHASE 6.4: GAUSSIAN CONVOLUTION (FULL VECTORIZATION) ===" << std::endl;
    
    double t0 = get_time_ms();
    for (int i = 0; i < RUNS; i++) {
        Image result = gaussianBlur(input);
    }
    double t1 = get_time_ms();
    double scalar_time = (t1 - t0) / RUNS;
    
    t0 = get_time_ms();
    for (int i = 0; i < RUNS; i++) {
        Image result = gaussianBlurRVV(input);
    }
    t1 = get_time_ms();
    double rvv_m2_time = (t1 - t0) / RUNS;
    
    std::cout << "Scalar:  " << scalar_time << " ms (baseline)" << std::endl;
    std::cout << "RVV:     " << rvv_m2_time << " ms (" 
              << (100*(scalar_time-rvv_m2_time)/scalar_time) << "% faster)" << std::endl;
    std::cout << "\nData Widening Chain: u8 (mf2) → u16 (m1) → i32 (m2)" << std::endl;
    std::cout << "Register Pressure: LMUL=2 balances throughput vs register availability" << std::endl;
    std::cout << "Pre-padding: Eliminates boundary checks, enables vectorization" << std::endl;

    return 0;
}
