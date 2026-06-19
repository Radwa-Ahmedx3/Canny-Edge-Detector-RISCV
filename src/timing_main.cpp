#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include <iostream>
#include <sys/time.h>
#include <fstream>

static unsigned char real_data[128*128];

void load_real_image() {
    std::ifstream f("real_image.raw", std::ios::binary);
    f.read((char*)real_data, 128*128);
}

double get_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

int main() {
    load_real_image();
    Image input(128, 128);
    for (int i = 0; i < 128*128; i++)
        input.getData()[i] = real_data[i];
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
