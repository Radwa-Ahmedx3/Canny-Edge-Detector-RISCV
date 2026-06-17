#include "image.hpp"
#include <cstring>
#include <iostream>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

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
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cerr << "Cannot open: " << filename << std::endl;
        return false;
    }
    read(fd, data, width * height);
    close(fd);
    return true;
}
bool Image::save(const std::string& filename) const {
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        std::cerr << "Cannot save: " << filename <<" errno="<<errno<<std::endl;
        return false;
    }
    write(fd, data, width * height);
    close(fd);
    return true;
}
