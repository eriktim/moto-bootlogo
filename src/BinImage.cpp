#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "BinImage.h"

BinImage::BinImage(char *data, size_t size)
{
    _data = new char[size];
    memcpy(_data, data, size);
    _size = size;
}

BinImage::~BinImage()
{
    delete[] _data;
    _data = NULL;
    if (_raw) {
        delete _raw;
        _raw = NULL;
    }
}

void BinImage::create_png(void)
{
    // check if the image data starts with 'MotoRun'
    if (strcmp(_data, "MotoRun")) {
        cerr << "Could not find valid image data" << endl;
        return;
    }
    _g = 8; // skip 8 bytes

    // read image dimensions
    _width = _read_dimension();
    _height = _read_dimension();

    if (!_width || !_height) {
        cerr << "Could not determine image size" << endl;
        return;
    }
    cout << "Image size: " << _width << " x " << _height << endl;

    _decode();
    if (!_raw) {
        cerr << "Failed decoding binary image" << endl;
        return;
    }
}

void BinImage::_decode(void)
{
    _raw = tmpfile();
    while (_g < _size) {
        uint8_t byte = _data[_g++];
        if (!(byte & 0x70)) {
            uint8_t mode = byte >> 7;
            int runlength = 0;
            runlength += byte & 0x0f << 16;
            runlength += _data[_g++] << 8;
            runlength += _data[_g++];

            char data[3];
            if (mode == 1) {
                // Repeat RGB value L times (1 color)
                // 0x8L LL R1 G1 B1
                data[0] = _data[_g++];
                data[1] = _data[_g++];
                data[2] = _data[_g++];
                for (int i = 0; i < runlength; i++) {
                    fwrite(data, sizeof(char), 3, _raw);
                }
            } else {
                // Read L RGB values (L colors)
                // 0x0L LL R1 G1 B1 R2 G2 B2 R3 G3 B3 ...
                for (int i = 0; i < runlength; i++) {
                    data[0] = _data[_g++];
                    data[1] = _data[_g++];
                    data[2] = _data[_g++];
                    fwrite(data, sizeof(char), 3, _raw);
                }
            }
        } else {
            // skip this byte
        }
    }
}

size_t BinImage::_read_dimension(void)
{
    char bytes[2];
    bytes[0] = _data[_g++];
    bytes[1] = _data[_g++];
    size_t value = 0;
    for (int i=0; i<2; i++) {
        value += pow(256, 1 - i) * (uint8_t)bytes[i];
    }
    return value;
}
