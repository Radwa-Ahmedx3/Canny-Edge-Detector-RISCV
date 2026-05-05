#include "image.hpp"
#include <iostream>
#include <malloc.h> 

Image::Image(int w, int h) : width(w), height(h) {
 
   data = (uint8_t*)memalign(64, width * height);
    
    if (data == nullptr) {
        std::cerr << "Allocation Failed!" << std::endl;
    }
}

Image::~Image() {
    if (data != nullptr) {
        free(data);
    }
}
