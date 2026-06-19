#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include "real_data.hpp"
#include <iostream>
#include <sys/time.h>

double get_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

int main() {
    Image input(real_w, real_h);
    for (int i = 0; i < real_w * real_h; i++)
        input.getData()[i] = real_image_data[i];
    std::cout << "Real image loaded!" << std::endl;
    double t1, t2;
    t1 = get_ms();
    for (int i = 0; i < 100; i++) gaussianBlur(input);
    Image blurred = gaussianBlur(input);
    t2 = get_ms();
    std::cout << "Gaussian: " << (t2-t1)/100.0 << " ms" << std::endl;
    t1 = get_ms();
    for (int i = 0; i < 100; i++) sobelGradient(blurred);
    t2 = get_ms();
    std::cout << "Sobel: " << (t2-t1)/100.0 << " ms" << std::endl;
    std::cout << "Done!" << std::endl;
    return 0;
}
