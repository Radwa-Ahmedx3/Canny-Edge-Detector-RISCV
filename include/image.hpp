#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <cstdint>
#include <cstdlib>

class Image {
private:
    int width, height;
    uint8_t* data;

public:
    Image(int w, int h);
    ~Image();
    uint8_t* getData() { return data; }
};

#endif
