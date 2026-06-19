#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include "test_data.hpp"
#include <iostream>
#include <fstream>

int main() {
    Image input(test_w, test_h);
    for (int i = 0; i < test_w * test_h; i++)
        input.getData()[i] = test_image[i];

    Image blurred = gaussianBlur(input);
    blurred.save("output/blurred.raw");
    std::cout << "Blurred saved!" << std::endl;

    SobelResult result = sobelGradient(blurred);
    result.magnitude.save("output/magnitude.raw");
    std::cout << "Magnitude saved!" << std::endl;

    return 0;
}
