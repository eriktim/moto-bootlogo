#ifndef BINIMAGE_H
#define BINIMAGE_H

using namespace std;

class BinImage
{
public:
    BinImage(char *data, size_t size);
    ~BinImage();
    void create_png(void);

private:
    size_t _size;
    size_t _width;
    size_t _height;
    char *_data;
};

#endif
