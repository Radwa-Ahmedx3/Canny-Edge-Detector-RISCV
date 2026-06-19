#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include "sobel.hpp"
#include "sobel_rvv.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

// Generate test image in memory: vertical edge
Image generateTestImage() {
    Image img(64, 64);
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            img.setPixel(x, y, x < 32 ? 0 : 255);
        }
    }
    return img;
}

// Save raw image using syscalls
void saveImage(const char* filename, const Image& img) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        std::cout << "Error opening " << filename << "\n";
        return;
    }
    write(fd, img.getData(), img.getWidth() * img.getHeight());
    close(fd);
    std::cout << "Saved: " << filename << "\n";
}

int main() {
    std::cout << "=== Canny Edge Detection (RISC-V + RVV) ===\n\n";

    std::cout << "Generating test image (64x64 vertical edge)...\n";
    Image input = generateTestImage();

    std::cout << "\n--- Scalar Pipeline ---\n";
    Image blur_s = gaussianBlur(input);
    SobelResult sobel_s = sobelGradient(blur_s);
    Image nms_s = nonMaxSuppression(sobel_s.magnitude, sobel_s.direction);
    Image edges_s = hysteresisThreshold(nms_s, 20, 60);
    saveImage("output_scalar.raw", edges_s);

    std::cout << "\n--- RVV Pipeline ---\n";
    Image blur_v = gaussianBlurRVV(input);
    SobelResult sobel_v = sobelGradientRVV(blur_v);
    Image nms_v = nonMaxSuppression(sobel_v.magnitude, sobel_v.direction);
    Image edges_v = hysteresisThreshold(nms_v, 20, 60);
    saveImage("output_rvv.raw", edges_v);

    std::cout << "\n=== Complete ===\n";
    return 0;
}
