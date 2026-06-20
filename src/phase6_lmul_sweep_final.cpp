#include "image.hpp"
#include <iostream>
#include <sys/time.h>
#include <cstring>
#include <iomanip>

Image gaussianBlurRVV(const Image& input);
Image gaussianBlurRVV_m1(const Image& input);
Image gaussianBlurRVV_m4(const Image& input);
Image gaussianBlurRVV_fp(const Image& input);

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
    std::cout << "\n=== PHASE 6.2: LMUL SWEEP ===" << std::endl;
    
    // LMUL=1
    double t0 = get_time_ms();
    for (int i = 0; i < RUNS; i++) {
        Image result = gaussianBlurRVV_m1(input);
    }
    double t1 = get_time_ms();
    double time_m1 = (t1 - t0) / RUNS;
    
    // LMUL=2 (baseline)
    t0 = get_time_ms();
    for (int i = 0; i < RUNS; i++) {
        Image result = gaussianBlurRVV(input);
    }
    t1 = get_time_ms();
    double time_m2 = (t1 - t0) / RUNS;
    
    // LMUL=4
    t0 = get_time_ms();
    for (int i = 0; i < RUNS; i++) {
        Image result = gaussianBlurRVV_m4(input);
    }
    t1 = get_time_ms();
    double time_m4 = (t1 - t0) / RUNS;
    
    // Fixed-point (LMUL=2 with div optimization)
    t0 = get_time_ms();
    for (int i = 0; i < RUNS; i++) {
        Image result = gaussianBlurRVV_fp(input);
    }
    t1 = get_time_ms();
    double time_fp = (t1 - t0) / RUNS;
    
    std::cout << "LMUL=1:     " << time_m1 << " ms" << std::endl;
    std::cout << "LMUL=2:     " << time_m2 << " ms (baseline)" << std::endl;
    std::cout << "LMUL=4:     " << time_m4 << " ms";
    if (time_m4 < time_m2) std::cout << " ← faster";
    else std::cout << " (register spilling)";
    std::cout << std::endl;
    std::cout << "Fixed-point:" << time_fp << " ms";
    if (time_fp < time_m2) std::cout << " ← fastest!";
    std::cout << std::endl;

    return 0;
}
