#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include <iostream>
#include <cstdio>

int main() {
    std::cout << "Test 1: Create 64x64 image\n";
    Image img(64, 64);
    std::cout << "Created\n";

    std::cout << "Test 2: Fill with value 128\n";
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            img.setPixel(x, y, 128);
        }
    }
    std::cout << "Filled\n";

    std::cout << "Test 3: Blur with scalar\n";
    Image blurred = gaussianBlur(img);
    std::cout << "Success\n";

    std::cout << "Test 4: Blur with RVV\n";
    Image blurred_rvv = gaussianBlurRVV(img);
    std::cout << "Success\n";

    std::cout << "All tests passed!\n";
    return 0;
}
