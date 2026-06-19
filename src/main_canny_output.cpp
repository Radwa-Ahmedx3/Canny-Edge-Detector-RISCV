#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include "sobel.hpp"
#include "sobel_rvv.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
#include <iostream>
#include <cstdio>

void outputImagePPM(const char* label, const Image& img) {
    printf("=== %s ===\n", label);
    printf("P2\n%d %d\n255\n", img.getWidth(), img.getHeight());
    for (int i = 0; i < img.getWidth() * img.getHeight(); i++) {
        printf("%d ", img.getData()[i]);
        if ((i+1) % 16 == 0) printf("\n");
    }
    printf("\n");
}

int main() {
    std::cout << "Generating test image...\n";
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, x < 32 ? 0 : 255);

    std::cout << "Processing with Scalar pipeline...\n";
    Image blur_s = gaussianBlur(img);
    SobelResult sr_s = sobelGradient(blur_s);
    Image edges_s = hysteresisThreshold(nonMaxSuppression(sr_s.magnitude, sr_s.direction), 20, 60);
    outputImagePPM("SCALAR OUTPUT", edges_s);

    std::cout << "Processing with RVV pipeline...\n";
    Image blur_v = gaussianBlurRVV(img);
    SobelResult sr_v = sobelGradientRVV(blur_v);
    Image edges_v = hysteresisThreshold(nonMaxSuppression(sr_v.magnitude, sr_v.direction), 20, 60);
    outputImagePPM("RVV OUTPUT", edges_v);

    std::cout << "Complete\n";
    return 0;
}
