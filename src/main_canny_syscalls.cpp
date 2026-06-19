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

// Load raw image from file
Image loadRawImage(const char* filename, int w, int h) {
    Image img(w, h);
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        std::cout << "Error: cannot open " << filename << "\n";
        return img;
    }
    read(fd, img.getData(), w * h);
    close(fd);
    std::cout << "Loaded: " << filename << " (" << w << "x" << h << ")\n";
    return img;
}

// Save raw image to file
void saveRawImage(const char* filename, const Image& img) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        std::cout << "Error: cannot write " << filename << "\n";
        return;
    }
    write(fd, img.getData(), img.getWidth() * img.getHeight());
    close(fd);
    std::cout << "Saved: " << filename << "\n";
}

int main() {
    std::cout << "=== Canny Edge Detection (RISC-V) ===\n\n";

    // Load input image
    std::cout << "Loading input image...\n";
    Image input = loadRawImage("test_input.raw", 64, 64);

    // Scalar pipeline
    std::cout << "\n--- Scalar Pipeline ---\n";
    std::cout << "Gaussian blur...\n";
    Image blur_s = gaussianBlur(input);
    
    std::cout << "Sobel gradient...\n";
    SobelResult sobel_s = sobelGradient(blur_s);
    
    std::cout << "Non-max suppression...\n";
    Image nms_s = nonMaxSuppression(sobel_s.magnitude, sobel_s.direction);
    
    std::cout << "Hysteresis...\n";
    Image edges_s = hysteresisThreshold(nms_s, 20, 60);
    
    saveRawImage("output_scalar.raw", edges_s);

    // RVV pipeline
    std::cout << "\n--- RVV Pipeline ---\n";
    std::cout << "Gaussian blur (RVV)...\n";
    Image blur_v = gaussianBlurRVV(input);
    
    std::cout << "Sobel gradient (RVV)...\n";
    SobelResult sobel_v = sobelGradientRVV(blur_v);
    
    std::cout << "Non-max suppression...\n";
    Image nms_v = nonMaxSuppression(sobel_v.magnitude, sobel_v.direction);
    
    std::cout << "Hysteresis...\n";
    Image edges_v = hysteresisThreshold(nms_v, 20, 60);
    
    saveRawImage("output_rvv.raw", edges_v);

    std::cout << "\n=== Complete ===\n";
    return 0;
}
