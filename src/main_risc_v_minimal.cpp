#include <iostream>
#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include "sobel.hpp"
#include "sobel_rvv.hpp"

int main() {
    std::cout << "Creating image...\n";
    Image img(32, 32);
    for (int i = 0; i < 32*32; i++) img.setPixel(i % 32, i / 32, 128);
    
    std::cout << "Gaussian...\n";
    Image blur = gaussianBlur(img);
    
    std::cout << "Sobel scalar...\n";
    SobelResult sr = sobelGradient(blur);
    std::cout << "Sobel OK\n";
    
    std::cout << "Sobel RVV...\n";
    SobelResult sr_rvv = sobelGradientRVV(blur);
    std::cout << "Sobel RVV OK\n";
    
    return 0;
}
