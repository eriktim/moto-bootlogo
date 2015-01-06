#ifndef BINIMAGE_H
#define BINIMAGE_H

using namespace std;

class BinImage
{
public:
    BinImage(char *data, int size);

private:
    int _size;
    int _width;
    int _height;
    char *_data;
};

#endif
