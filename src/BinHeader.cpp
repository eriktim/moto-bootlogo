#include "BinHeader.h"

BinHeader::BinHeader(size_t offset, size_t size, size_t g, BinImage *image)
{
    _offset = offset;
    _size = size;
    _g = g;
    _image = image;
}

BinHeader::~BinHeader()
{
    if (_image) {
        delete _image;
        _image = NULL;
    }
}

BinImage *BinHeader::get_image(void)
{
    return _image;
}

size_t BinHeader::get_g(void)
{
    return _g;
}

size_t BinHeader::get_offset(void)
{
    return _offset;
}

size_t BinHeader::get_size(void)
{
    return _size;
}
