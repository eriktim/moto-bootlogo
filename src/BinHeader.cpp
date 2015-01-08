#include "BinHeader.h"

BinHeader::BinHeader(int offset, int size, BinImage *image)
{
    _offset = offset;
    _size = size;
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
