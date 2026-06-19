#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include "test_data.hpp"
#include <iostream>

int main() {
    // load image from embedded data
    Image input(test_w, test_h);
    for (int i = 0; i < test_w * test_h; i++)
        input.getData()[i] = test_image[i];
    
    std::cout << "Image loaded from memory!" << std::endl;

    // Gaussian Blur
    Image blurred = gaussianBlur(input);
    std::cout << "Gaussian Blur done!" << std::endl;

    // Sobel Gradient
    SobelResult result = sobelGradient(blurred);
    std::cout << "Sobel done!" << std::endl;

    // print some pixel values to verify
    std::cout << "Magnitude pixel [64][64] = " 
              << (int)result.magnitude.getPixel(64,64) << std::endl;
    std::cout << "Magnitude pixel [30][30] = " 
              << (int)result.magnitude.getPixel(30,30) << std::endl;
    std::cout << "All stages passed!" << std::endl;
    return 0;
}
