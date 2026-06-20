// Scalar vs RVV equivalence + timing test, per project spec section 3.2 / 7.1.
// Uses a non-power-of-two image (100x75) to force the strip-mining
// tail case (spec's "Common Pitfalls": power-of-two sizes hide bugs).
// Run this same binary at VLEN=128, 256, and 512 (-cpu rv64,v=true,vlen=N)
// to confirm RVV output is identical across vector lengths, and to see
// per-stage scalar vs RVV timing at each VLEN.

#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include <iostream>
#include <cstdlib>
#include <sys/time.h>

static double get_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

static Image makeTestImage(int w, int h) {
    Image img(w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            uint8_t val = (uint8_t)(((x * 37 + y * 53) ^ (x / 5 + y / 7)) & 0xFF);
            img.setPixel(x, y, val);
        }
    }
    return img;
}

static bool compareImages(const Image& a, const Image& b, const char* name, int tolerance) {
    int w = a.getWidth(), h = a.getHeight();
    int maxDiff = 0, mismatches = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int diff = abs((int)a.getPixel(x, y) - (int)b.getPixel(x, y));
            if (diff > maxDiff) maxDiff = diff;
            if (diff > tolerance) mismatches++;
        }
    }
    bool pass = (mismatches == 0);
    std::cout << name << ": max_diff=" << maxDiff
               << " mismatches=" << mismatches << "/" << (w * h)
               << (pass ? "  [PASS]" : "  [FAIL]") << "\n";
    return pass;
}

int main() {
    const int W = 100, H = 75;
    const int RUNS = 100;
    std::cout << "=== Scalar vs RVV Equivalence + Timing Test (" << W << "x" << H << ", non-power-of-two) ===\n";

    Image input = makeTestImage(W, H);

    Image blurScalar = gaussianBlur(input);
    Image blurRVV    = gaussianBlurRVV(input);
    bool gaussPass = compareImages(blurScalar, blurRVV, "Gaussian (scalar vs RVV)", 1);

    SobelResult sobelScalar = sobelGradient(blurScalar);
    SobelResult sobelRVV    = sobelGradientRVV(blurRVV);
    bool magPass = compareImages(sobelScalar.magnitude, sobelRVV.magnitude, "Sobel magnitude (scalar vs RVV)", 1);
    bool dirPass = compareImages(sobelScalar.direction, sobelRVV.direction, "Sobel direction (scalar vs RVV)", 0);

    bool allPass = gaussPass && magPass && dirPass;
    std::cout << (allPass ? "ALL EQUIVALENCE TESTS PASSED\n" : "SOME EQUIVALENCE TESTS FAILED\n");

    double gScalarTotal = 0, gRvvTotal = 0, sScalarTotal = 0, sRvvTotal = 0;
    for (int i = 0; i < RUNS; i++) {
        double t0 = get_ms();
        Image bs = gaussianBlur(input);
        double t1 = get_ms();
        Image br = gaussianBlurRVV(input);
        double t2 = get_ms();
        SobelResult ss = sobelGradient(bs);
        double t3 = get_ms();
        SobelResult sr = sobelGradientRVV(br);
        double t4 = get_ms();

        gScalarTotal += t1 - t0;
        gRvvTotal    += t2 - t1;
        sScalarTotal += t3 - t2;
        sRvvTotal    += t4 - t3;
    }

    double gScalarAvg = gScalarTotal / RUNS;
    double gRvvAvg    = gRvvTotal / RUNS;
    double sScalarAvg = sScalarTotal / RUNS;
    double sRvvAvg    = sRvvTotal / RUNS;

    std::cout << "\n=== Timing (avg over " << RUNS << " runs, " << W << "x" << H << ") ===\n";
    std::cout << "Gaussian  scalar: " << gScalarAvg << " ms   RVV: " << gRvvAvg
               << " ms   speedup: " << (gScalarAvg / gRvvAvg) << "x\n";
    std::cout << "Sobel     scalar: " << sScalarAvg << " ms   RVV: " << sRvvAvg
               << " ms   speedup: " << (sScalarAvg / sRvvAvg) << "x\n";

    return allPass ? 0 : 1;
}
