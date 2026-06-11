#include "image.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <malloc.h>

Image::Image(int w, int h) : width(w), height(h) {
    data = (uint8_t*)memalign(64, width * height);
    if (data == nullptr) {
        std::cerr << "Allocation Failed!" << std::endl;
    }
}

Image::Image(const Image& other) : width(other.width), height(other.height) {
    data = (uint8_t*)memalign(64, width * height);
    memcpy(data, other.data, width * height);
}

Image& Image::operator=(const Image& other) {
    if (this != &other) {
        free(data);
        width = other.width;
        height = other.height;
        data = (uint8_t*)memalign(64, width * height);
        memcpy(data, other.data, width * height);
    }
    return *this;
}

Image::~Image() {
    if (data != nullptr) {
        free(data);
    }
}

uint8_t Image::getPixel(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return 0;
    return data[y * width + x];
}

void Image::setPixel(int x, int y, uint8_t val) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    data[y * width + x] = val;
}

bool Image::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open: " << filename << std::endl;
        return false;
    }
    file.read((char*)data, width * height);
    return true;
}

bool Image::save(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot save: " << filename << std::endl;
        return false;
    }
    file.write((char*)data, width * height);
    return true;
}