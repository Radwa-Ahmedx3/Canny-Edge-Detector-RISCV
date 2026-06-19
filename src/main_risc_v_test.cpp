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

// Generate synthetic test image: vertical edge
Image generateVerticalEdge(int w, int h) {
    Image img(w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            img.setPixel(x, y, x < w / 2 ? 0 : 255);
        }
    }
    return img;
}

// Save image to .raw file
void saveImage(const char* filename, const Image& img) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        std::cout << "Error opening " << filename << "\n";
        return;
    }
    int w = img.getWidth(), h = img.getHeight();
    write(fd, img.getData(), w * h);
    close(fd);
    std::cout << "Saved: " << filename << " (" << w << "x" << h << ")\n";
}

int main() {
    std::cout << "=== RISC-V Canny Edge Detection ===\n";
    std::cout << "Image size: 128x128\n\n";

    // Generate test image
    Image test = generateVerticalEdge(128, 128);
    std::cout << "Generated vertical edge image\n";

    // Scalar pipeline
    std::cout << "\nScalar Gaussian...\n";
    Image blur_scalar = gaussianBlur(test);
    saveImage("edge_scalar_blur.raw", blur_scalar);

    std::cout << "Scalar Sobel...\n";
    SobelResult sobel_scalar = sobelGradient(blur_scalar);
    saveImage("edge_scalar_magnitude.raw", sobel_scalar.magnitude);
    saveImage("edge_scalar_direction.raw", sobel_scalar.direction);

    // RVV pipeline
    std::cout << "\nRVV Gaussian...\n";
    Image blur_rvv = gaussianBlurRVV(test);
    saveImage("edge_rvv_blur.raw", blur_rvv);

    std::cout << "RVV Sobel...\n";
    SobelResult sobel_rvv = sobelGradientRVV(blur_rvv);
    saveImage("edge_rvv_magnitude.raw", sobel_rvv.magnitude);
    saveImage("edge_rvv_direction.raw", sobel_rvv.direction);

    std::cout << "\n=== All outputs saved ===\n";
    return 0;
}
