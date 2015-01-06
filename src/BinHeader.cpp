#include "BinHeader.h"

BinHeader::BinHeader(int offset, int size, BinImage *image)
{
    _offset = offset;
    _size = size;
    _image = image;
}

BinImage *BinHeader::get_image(void)
{
    return _image;
}
