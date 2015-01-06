#ifndef BINHEADER_H
#define BINHEADER_H

#include "BinFile.h"
#include "BinImage.h"

using namespace std;

class BinFile;

class BinHeader
{
public:
    BinHeader(int offset, int size, BinFile *file);

private:
    int _offset;
    int _size;
    BinFile *_file;
    BinImage *_image;
};

#endif
