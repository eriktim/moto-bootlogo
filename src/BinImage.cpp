#include <iostream>
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
}
