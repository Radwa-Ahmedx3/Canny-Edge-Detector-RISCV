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

void saveImage(const char* path, const Image& img) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    std::cout << "Opening " << path << " -> fd=" << fd << "\n";
    if (fd < 0) return;
    write(fd, img.getData(), img.getWidth() * img.getHeight());
    close(fd);
    std::cout << "Saved: " << path << "\n";
}

int main() {
    std::cout << "=== Canny (RISC-V) ===\n";
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, x < 32 ? 0 : 255);

    std::cout << "Scalar...\n";
    Image blur_s = gaussianBlur(img);
    SobelResult sr_s = sobelGradient(blur_s);
    Image edges_s = hysteresisThreshold(nonMaxSuppression(sr_s.magnitude, sr_s.direction), 20, 60);
    saveImage("/tmp/output_scalar.raw", edges_s);

    std::cout << "RVV...\n";
    Image blur_v = gaussianBlurRVV(img);
    SobelResult sr_v = sobelGradientRVV(blur_v);
    Image edges_v = hysteresisThreshold(nonMaxSuppression(sr_v.magnitude, sr_v.direction), 20, 60);
    saveImage("/tmp/output_rvv.raw", edges_v);

    std::cout << "Done\n";
    return 0;
}
