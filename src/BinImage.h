#ifndef BINIMAGE_H
#define BINIMAGE_H

using namespace std;

class BinImage
{
public:
    BinImage(int size, char *data);

private:
    int _size;
    int _width;
    int _height;
    char *_data;
};

#endif
