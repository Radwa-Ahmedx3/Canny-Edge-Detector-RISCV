#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include "sobel.hpp"
#include "sobel_rvv.hpp"
#include <iostream>
#include <sys/time.h>

double get_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

int main() {
    int w = 128, h = 128;
    Image input(w, h);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            bool inSquare = (x>=30 && x<90 && y>=30 && y<90);
            input.setPixel(x, y, inSquare ? 255 : 0);
        }

    double t1, t2;

    std::cout << "=== Scalar ===" << std::endl;
    t1 = get_ms();
    for (int i = 0; i < 100; i++) gaussianBlur(input);
    t2 = get_ms();
    std::cout << "Gaussian scalar: " << (t2-t1)/100.0 << " ms" << std::endl;

    Image blurred = gaussianBlur(input);
    t1 = get_ms();
    for (int i = 0; i < 100; i++) sobelGradient(blurred);
    t2 = get_ms();
    std::cout << "Sobel scalar:    " << (t2-t1)/100.0 << " ms" << std::endl;

    std::cout << "\n=== RVV ===" << std::endl;
    t1 = get_ms();
    for (int i = 0; i < 100; i++) gaussianBlurRVV(input);
    t2 = get_ms();
    std::cout << "Gaussian RVV:    " << (t2-t1)/100.0 << " ms" << std::endl;

    Image rvv_blurred = gaussianBlurRVV(input);
    t1 = get_ms();
    for (int i = 0; i < 100; i++) sobelGradientRVV(rvv_blurred);
    t2 = get_ms();
    std::cout << "Sobel RVV:       " << (t2-t1)/100.0 << " ms" << std::endl;

    return 0;
}
