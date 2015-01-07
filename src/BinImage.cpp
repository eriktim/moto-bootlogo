#include <iostream>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "BinImage.h"

BinImage::BinImage(char *data, size_t size)
{
    _data = data;
    _size = size;
}

BinImage::~BinImage()
{
    delete[] _data;
    _data = NULL;
}

void BinImage::create_png(void)
{
    // check if the image data starts with 'MotoRun'
    if (strcmp(_data, "MotoRun")) {
        cerr << "Could not find valid image data" << endl;
    }
    _g = 8; // skip 8 bytes

    // read image dimensions
    _width = _read_value();
    _height = _read_value();

    if (!_width || !_height) {
        cerr << "Could not determine image size" << endl;
    }
    cout << "Image size: " << _width << " x " << _height << endl;
}

size_t BinImage::_read_value(void) {
    char bytes[2];
    bytes[0] = _data[_g++];
    bytes[1] = _data[_g++];
    size_t value = 0;
    for (int i=0; i<2; i++) {
        value += pow(256, 1 - i) * (uint8_t)bytes[i];
    }
    return value;
}
