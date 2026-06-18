#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <cstdint>
#include <cstdlib>
#include <string>

class Image {
private:
    int width, height;
    uint8_t* data;

public:
    Image(int w, int h);
    Image(const Image& other);
    Image& operator=(const Image& other);
    ~Image();

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    uint8_t* getData() const { return data; }
    const uint8_t* getConstData() const { return data; }

    uint8_t getPixel(int x, int y) const;
    void setPixel(int x, int y, uint8_t val);

    bool load(const std::string& filename);
    bool save(const std::string& filename) const;
};

#endif
