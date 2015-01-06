#include "BinHeader.h"

BinHeader::BinHeader(int offset, int size, BinFile *file)
{
    _offset = offset;
    _size = size;
    _file = file;
    _image = NULL;
}
