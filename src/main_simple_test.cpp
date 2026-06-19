#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include "sobel.hpp"
#include "sobel_rvv.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
#include <iostream>

int main() {
    std::cout << "Creating image...\n";
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, x < 32 ? 0 : 255);
    
    std::cout << "Gaussian + Sobel + NMS...\n";
    Image blur = gaussianBlur(img);
    SobelResult sr = sobelGradient(blur);
    Image nms = nonMaxSuppression(sr.magnitude, sr.direction);
    std::cout << "OK\n";
    
    std::cout << "Hysteresis...\n";
    Image edges = hysteresisThreshold(nms, 20, 60);
    std::cout << "OK\n";
    
    std::cout << "Done!\n";
    return 0;
}
