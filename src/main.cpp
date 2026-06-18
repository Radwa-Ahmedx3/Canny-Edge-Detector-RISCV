#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
#include <iostream>
#include <sys/time.h>

double get_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

void process_image(const char* input_file, const char* output_nms, const char* output_edges, int width, int height) {
    std::cout << "\n=== Processing: " << input_file << " ===" << std::endl;

    Image input(width, height);
    if (!input.load(input_file)) {
        std::cout << "Failed to load image!" << std::endl;
        return;
    }

    double t1, t2;

    // Gaussian Blur
    t1 = get_ms();
    for (int i = 0; i < 100; i++) gaussianBlur(input);
    Image blurred = gaussianBlur(input);
    t2 = get_ms();
    std::cout << "Gaussian: " << (t2 - t1) / 100.0 << " ms" << std::endl;

    // Sobel
    t1 = get_ms();
    for (int i = 0; i < 100; i++) sobelGradient(blurred);
    SobelResult result = sobelGradient(blurred);
    t2 = get_ms();
    std::cout << "Sobel: " << (t2 - t1) / 100.0 << " ms" << std::endl;

    // NMS
    t1 = get_ms();
    for (int i = 0; i < 100; i++) nonMaxSuppression(result.magnitude, result.direction);
    Image thinned = nonMaxSuppression(result.magnitude, result.direction);
    t2 = get_ms();
    std::cout << "NMS: " << (t2 - t1) / 100.0 << " ms" << std::endl;

    // Hysteresis
    t1 = get_ms();
    for (int i = 0; i < 100; i++) hysteresisThreshold(thinned, 20, 80);
    Image edges = hysteresisThreshold(thinned, 20, 80);
    t2 = get_ms();
    std::cout << "Hysteresis: " << (t2 - t1) / 100.0 << " ms" << std::endl;

    thinned.save(output_nms);
    edges.save(output_edges);
}

int main() {
    process_image("test_square.raw", "/tmp/square_nms.raw", "/tmp/square_edges.raw", 128, 128);
    process_image("test_shapes.raw", "/tmp/shapes_nms.raw", "/tmp/shapes_edges.raw", 128, 128);
    process_image("test_real.raw", "/tmp/real_nms.raw", "/tmp/real_edges.raw", 128, 128);
    return 0;
}
