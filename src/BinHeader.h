#ifndef BINHEADER_H
#define BINHEADER_H

#include "BinFile.h"
#include "BinImage.h"

using namespace std;

class BinFile;

class BinHeader
{
public:
    BinHeader(int offset, int size, BinImage *image);
    BinImage *get_image(void);

private:
    int _offset;
    int _size;
    BinImage *_image;
};

#endif
