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
    const int RUNS = 100;
    double g_total = 0, s_total = 0, n_total = 0, h_total = 0;
    Image blurred(width, height), thinned(width, height), edges(width, height);
    SobelResult result;
    for (int i = 0; i < RUNS; i++) {
        double t0 = get_ms();
        blurred = gaussianBlurRVV(input);
        double t1 = get_ms();
        result = sobelGradientRVV(blurred);
        double t2 = get_ms();
        thinned = nonMaxSuppression(result.magnitude, result.direction);
        double t3 = get_ms();
        edges = hysteresisThreshold(thinned, 20, 60);
        double t4 = get_ms();
        g_total += t1 - t0;
        s_total += t2 - t1;
        n_total += t3 - t2;
        h_total += t4 - t3;
    }
    double avg_g = g_total / RUNS;
    double avg_s = s_total / RUNS;
    double avg_n = n_total / RUNS;
    double avg_h = h_total / RUNS;
    double avg_total = avg_g + avg_s + avg_n + avg_h;
    std::cout << "Averaged over " << RUNS << " runs:\n";
    std::cout << "-------------------------\n";
    std::cout << "Gaussian:   " << avg_g << " ms  (" << (avg_g/avg_total*100) << "%)\n";
    std::cout << "Sobel:      " << avg_s << " ms  (" << (avg_s/avg_total*100) << "%)\n";
    std::cout << "NMS:        " << avg_n << " ms  (" << (avg_n/avg_total*100) << "%)\n";
    std::cout << "Hysteresis: " << avg_h << " ms  (" << (avg_h/avg_total*100) << "%)\n";
    std::cout << "-------------------------\n";
    std::cout << "Total:      " << avg_total << " ms\n";
    blurred.save("./blurred.raw");
    thinned.save(output_nms);
    edges.save(output_edges);
}
int main() {
    process_image("test_square.raw", "/tmp/square_nms.raw", "/tmp/square_edges.raw", 128, 128);
    process_image("test_shapes.raw", "/tmp/shapes_nms.raw", "/tmp/shapes_edges.raw", 128, 128);
    process_image("test_real.raw", "/tmp/real_nms.raw", "/tmp/real_edges.raw", 128, 128);
    process_image("test_car.raw", "/tmp/car_nms.raw", "/tmp/car_edges.raw", 512, 512);
    return 0;
}
