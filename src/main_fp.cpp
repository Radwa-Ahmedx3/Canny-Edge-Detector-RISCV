#include "image.hpp"
#include "gaussian.hpp"
#include <iostream>
#include <cmath>
#include <sys/time.h>

double get_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

int main() {
    Image input(128, 128);
    if (!input.load("./test_input.raw")) return 1;

    const int RUNS = 100;
    double t_div=0, t_fp=0;
    int max_diff = 0, diff_count = 0;

    for (int i = 0; i < RUNS; i++) {
        double t0 = get_ms();
        Image b_div = gaussianBlurRVV(input);
        t_div += get_ms() - t0;

        double t1 = get_ms();
        Image b_fp = gaussianBlurRVV_fp(input);
        t_fp += get_ms() - t1;

        if (i == 0) {
            for (int y = 0; y < 128; y++) {
                for (int x = 0; x < 128; x++) {
                    int diff = abs((int)b_div.getPixel(x,y) - (int)b_fp.getPixel(x,y));
                    if (diff > max_diff) max_diff = diff;
                    if (diff > 0) diff_count++;
                }
            }
        }
    }

    std::cout << "=== Fixed-Point vs Integer Division ===\n";
    std::cout << "vdiv (exact):       " << t_div/RUNS << " ms\n";
    std::cout << "fixed-point (*240>>16): " << t_fp/RUNS << " ms\n";
    std::cout << "Speedup: " << t_div/t_fp << "x\n";
    std::cout << "Max pixel difference: " << max_diff << "\n";
    std::cout << "Pixels with any difference: " << diff_count << " / 16384\n";
    return 0;
}
