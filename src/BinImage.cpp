#include <iomanip>
#include <iostream>
#include <png.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "BinImage.h"

#define PNG_DEBUG 3

BinImage::BinImage(char *data, size_t size)
{
    // check if the image data starts with 'MotoRun'
    if (strcmp(data, "MotoRun")) {
        cerr << "Could not find valid image data" << endl;
        return;
    }
    _g = 8; // 'MotoRun '

    _data = new uint8_t[size];
    memcpy(_data, data, size);
    _size = size;

    // read image dimensions
    _width = _read_dimension();
    _height = _read_dimension();

    if (!_width || !_height) {
        cerr << "Could not determine image size" << endl;
        return;
    }
}

BinImage::~BinImage()
{
    if (_data) {
        delete[] _data;
        _data = NULL;
    }
    if (_raw) {
        delete[] _raw;
        _raw = NULL;
    }
}

void BinImage::create_png(string filename)
{
    cout << "Image size: " << _width << " x " << _height << endl;

    _decode();
    if (!_raw) {
        cerr << "Failed decoding binary image" << endl;
        return;
    }

    FILE *fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        cerr << "Could not open file for writing PNG image" << endl;
        return;
    }

    // allocate & initialize PNG struct
    png_structp pngStruct = png_create_write_struct(
            PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    if (!pngStruct) {
        cerr << "[PNG] Failed creating structure" << endl;
        return;
    }

    // allocate & initialize PNG info
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (!pngInfo) {
        cerr << "[PNG] Failed creating info" << endl;
        png_destroy_write_struct(&pngStruct, (png_infopp)NULL);
        fclose(fp);
        return;
    }

    // set-up error handling
    if (setjmp(png_jmpbuf(pngStruct))) {
        cerr << "[PNG] Failed writing file" << endl;
        png_destroy_write_struct(&pngStruct, &pngInfo);
        fclose(fp);
        return;
    }

    // initialize output
    png_init_io(pngStruct, fp);

    // write headers
    png_set_IHDR(pngStruct, pngInfo, _width, _height,
            8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // write data
    png_byte *pngRows[_height];
    for (size_t k = 0; k < _height; k++) {
        pngRows[k] = &_raw[k * _width * 3];
    }
    png_set_rows(pngStruct, pngInfo, pngRows);
    png_write_png(pngStruct, pngInfo, PNG_TRANSFORM_BGR, NULL);

    // cleanup
    png_destroy_write_struct(&pngStruct, &pngInfo);
    fclose(fp);
    delete[] _raw;
    _raw = NULL;
}

uint8_t *BinImage::get_data(void)
{
    return _data;
}

size_t BinImage::get_size(void)
{
    return _size;
}

bool BinImage::set_data_from_png(string filename)
{
    // try opening the file
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        cerr << "Failed opening '" << filename << "'" << endl;
        return false;
    }

    // validate header
    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        cerr << "[PNG] '" << filename
                << "' is not recognized as a PNG file" << endl;
        fclose(fp);
        return false;
    }

    // allocate & initialize PNG struct
    png_structp pngStruct = png_create_read_struct
            (PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    if (!pngStruct) {
        cerr << "[PNG] Failed creating structure" << endl;
        fclose(fp);
        return false;
    }

    // allocate & initialize PNG info
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (!pngInfo) {
        cerr << "[PNG] Failed creating info" << endl;
        png_destroy_write_struct(&pngStruct, (png_infopp)NULL);
        fclose(fp);
        return false;
    }

    // set-up error handling
    if (setjmp(png_jmpbuf(pngStruct))) {
        cerr << "[PNG] Failed reading file" << endl;
        png_destroy_write_struct(&pngStruct, &pngInfo);
        fclose(fp);
        return false;
    }

    // initialize input
    png_init_io(pngStruct, fp);

    // inform the library 8 bytes have been read
    png_set_sig_bytes(pngStruct, 8);

    // read up to image data, as we need to verify the info first
    png_read_info(pngStruct, pngInfo);

    // check whether the PNG specifications are met
    bool unsupported = false;
    long unsigned int width, height;
    int depth, type, interlaceType, compressionType, filterMethod;

    png_get_IHDR(pngStruct, pngInfo, &width, &height,
            &depth, &type, &interlaceType, &compressionType, &filterMethod);

    if (width != _width || height != _height) {
        cerr << "PNG dimensions do not match the original image" << endl;
        unsupported = true;
    }

    if (depth != 8) {
        cerr << "PNG depth should be 8" << endl;
        unsupported = true;
    }

    switch (type) {
        case PNG_COLOR_TYPE_RGB_ALPHA:
            png_set_strip_alpha(pngStruct);
            // do not break
        case PNG_COLOR_TYPE_RGB:
            png_set_bgr(pngStruct);
            break;
        default:
            cerr << "PNG type should be 'RGB'" << endl;
            unsupported = true;
    }

    if (interlaceType != PNG_INTERLACE_NONE) {
        cerr << "PNG should not be interlaced" << endl;
        unsupported = true;
    }

    if (compressionType != PNG_COMPRESSION_TYPE_BASE) {
        cerr << "PNG should have base compression" << endl;
        unsupported = true;
    }

    if (filterMethod != PNG_FILTER_TYPE_BASE) {
        cerr << "PNG should have base filtering" << endl;
        unsupported = true;
    }

    if (unsupported) {
        return false;
    }

    // update info with transformations
    png_read_update_info(pngStruct, pngInfo);

    // read the data from the PNG
    _raw = new uint8_t[3 * width * height];
    png_bytep pngRows[height];
    for (long unsigned int i = 0; i < height; ++i) {
        pngRows[i] = &_raw[i * 3 * width];
    }
    png_read_image(pngStruct, pngRows);

    // cleanup
    png_destroy_read_struct(&pngStruct, &pngInfo, NULL);
    fclose(fp);

    return _encode();
}

size_t BinImage::_block_length(bool repeat, size_t g)
{
    size_t max = _width - (g % _width);
    size_t l;
    bool warning;
    for (l = 1; l < max; ++l) {
        size_t j = 3 * (g + l);
        bool equal = true;
        for (size_t k = 0; k < 3; ++k) {
            if (_raw[j + k] != _raw[j + 3 + k]) {
                equal = false;
            }
        }
        if (equal) {
            if (!repeat) {
                if (warning) {
                    return l - 2;
                } else {
                    warning = true;
                }
            }
        } else {
            if (repeat) {
                return l;
            } else {
                warning = false;
            }
        }
    }
    return l;
}

void BinImage::_decode(void)
{
    _raw = new uint8_t[3 * _width * _height];
    size_t g = 0;

    while (_g < _size) {
        uint8_t byte = _data[_g++];
        if ((byte & 0x70) > 0) {
            // should not be reached
            cerr << "Invalid nybble (" << (int)byte
                    << ") at position " << _g << endl;
        } else {
            uint16_t runlength = 0;
            runlength += (byte & 0x0f) << 8;
            runlength += _data[_g++];

            // Repeat RGB value L times (1 color)
            // 0x8L LL R1 G1 B1
            //  (or)
            // Read L RGB values (L colors)
            // 0x0L LL R1 G1 B1 R2 G2 B2 R3 G3 B3 ...
            bool repeat = byte >> 7;

            for (uint16_t i = 0; i < runlength; ++i) {
                memcpy(&_raw[g], &_data[_g], 3);
                g += 3;
                if (!repeat) {
                    _g += 3;
                }
            }
            if (repeat) {
                _g += 3;
            }
        }
    }

    if (_g != _size) {
        cerr << "Incorrect file size (" << _g << "/" << _size << ")" << endl;
        delete[] _raw;
        _raw = NULL;
    }
}

bool BinImage::_encode(void)
{
    if (!_raw) {
        cerr << "No data to encode" << endl;
        return false;
    }

    string prefix("MotoRun");
    vector<uint8_t> data(prefix.begin(), prefix.end());
    data.push_back(0x00);
    data.push_back((_width >> 8) & 0xff);
    data.push_back(_width & 0xff);
    data.push_back((_height >> 8) & 0xff);
    data.push_back(_height & 0xff);
    uint8_t mode;

    for (size_t i = 0, L, length; i < _width * _height; i += length) {
        length = _block_length(true, i);
        if (length >= 3) {
            mode = 0x80;
            L = 1;
        } else {
            length = _block_length(false, i);
            mode = 0x00;
            L = length;
        }
        uint8_t nybble = (length >> 8) & 0x0f;
        data.push_back(mode | nybble);
        data.push_back(length & 0xff);

        for (size_t l = 1, j; l <= L; ++l) {
            j = 3 * (i + l);
            data.push_back(_raw[j + 0]);
            data.push_back(_raw[j + 1]);
            data.push_back(_raw[j + 2]);
        }
    }

    cout << "Encoded size: " << data.size() << " bytes" << endl;
    if (_data) {
        delete[] _data;
    }
    _size = data.size();
    _data = &data[0];

    return true;
}

size_t BinImage::_read_dimension(void)
{
    uint8_t bytes[2];
    bytes[0] = _data[_g++];
    bytes[1] = _data[_g++];
    uint16_t value = 0;
    for (int i = 0; i < 2; ++i) {
        value += bytes[i] << 8 * (1 - i);
    }
    return value;
}
