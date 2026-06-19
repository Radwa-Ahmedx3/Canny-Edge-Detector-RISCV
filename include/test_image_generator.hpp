#pragma once
#include "image.hpp"

class TestImageGenerator {
public:
    static Image blackImage(int w, int h) {
        Image img(w, h);
        for (int i = 0; i < w * h; i++)
            img.setPixel(i % w, i / w, 0);
        return img;
    }

    static Image uniformImage(int w, int h, uint8_t value) {
        Image img(w, h);
        for (int i = 0; i < w * h; i++)
            img.setPixel(i % w, i / w, value);
        return img;
    }

    static Image impulseImage(int w, int h) {
        Image img = blackImage(w, h);
        img.setPixel(w / 2, h / 2, 255);
        return img;
    }

    static Image verticalEdgeImage(int w, int h) {
        Image img(w, h);
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
                img.setPixel(x, y, x < w / 2 ? 0 : 255);
        return img;
    }

    static Image horizontalEdgeImage(int w, int h) {
        Image img(w, h);
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
                img.setPixel(x, y, y < h / 2 ? 0 : 255);
        return img;
    }

    static Image diagonalEdgeImage(int w, int h) {
        Image img(w, h);
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
                img.setPixel(x, y, x > y ? 255 : 0);
        return img;
    }
};
