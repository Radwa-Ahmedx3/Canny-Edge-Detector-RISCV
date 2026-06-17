#include "sobel.hpp"
#include <cstdlib>

// Sobel X kernel - بيكشف الـ edges الرأسية
static const int Kx[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

// Sobel Y kernel - بيكشف الـ edges الأفقية
static const int Ky[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

SobelResult sobelGradient(const Image& input) {
    int w = input.getWidth();
    int h = input.getHeight();

    Image magnitude(w, h);
    Image direction(w, h);

    int maxMag = 1;

    // مصفوفة مؤقتة للـ magnitude قبل الـ normalization
    int* mag = new int[w * h]();

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int gx = 0, gy = 0;

            // نطبق الـ Sobel kernels
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int px = x + kx;
                    int py = y + ky;
                    int pixel = 0;
                    if (px >= 0 && px < w && py >= 0 && py < h)
                        pixel = input.getPixel(px, py);
                    gx += pixel * Kx[ky+1][kx+1];
                    gy += pixel * Ky[ky+1][kx+1];
                }
            }

            // L1 norm: |Gx| + |Gy|
            int m = abs(gx) + abs(gy);
            mag[y * w + x] = m;
            if (m > maxMag) maxMag = m;

            // حساب الـ direction بدون atan2
            int ax = abs(gx), ay = abs(gy);
            uint8_t dir;
            if (ay * 5 < ax * 2)       dir = 0;  // 0 degrees
            else if (ay * 5 > ax * 12) dir = 2;  // 90 degrees
            else if (gx * gy > 0)      dir = 1;  // 45 degrees
            else                        dir = 3;  // 135 degrees

            direction.setPixel(x, y, dir);
        }
    }

    // normalize الـ magnitude لـ [0, 255]
    for (int i = 0; i < w * h; i++) {
        uint8_t val = (uint8_t)(mag[i] * 255 / maxMag);
        magnitude.setPixel(i % w, i / w, val);
    }

    delete[] mag;

    SobelResult result = {magnitude, direction};
    return result;
}
#include <cmath>

// L2 norm: sqrt(Gx^2 + Gy^2) — mathematically correct but requires floating point
SobelResult sobelGradientL2(const Image& input) {
    int w = input.getWidth(), h = input.getHeight();
    Image magnitude(w, h), direction(w, h);

    static const int Kx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    static const int Ky[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};

    float maxMag = 1.0f;
    float* mag = new float[w * h]();

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int gx = 0, gy = 0;
            for (int ky=-1; ky<=1; ky++)
                for (int kx=-1; kx<=1; kx++) {
                    int px=x+kx, py=y+ky;
                    int p = (px>=0&&px<w&&py>=0&&py<h) ? input.getPixel(px,py) : 0;
                    gx += p * Kx[ky+1][kx+1];
                    gy += p * Ky[ky+1][kx+1];
                }
            float m = sqrtf((float)(gx*gx + gy*gy));
            mag[y*w+x] = m;
            if (m > maxMag) maxMag = m;
            int ax=abs(gx), ay=abs(gy); uint8_t dir;
            if (ay*5 < ax*2)       dir = 0;
            else if (ay*5 > ax*12) dir = 2;
            else if (gx*gy > 0)    dir = 1;
            else                    dir = 3;
            direction.setPixel(x, y, dir);
        }
    }
    for (int i=0; i<w*h; i++)
        magnitude.setPixel(i%w, i/w, (uint8_t)(mag[i]*255.0f/maxMag));
    delete[] mag;
    SobelResult result = {magnitude, direction};
    return result;
}
