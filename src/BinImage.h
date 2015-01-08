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
    void _decode(void);
    size_t _read_dimension(void);

    size_t _size;
    size_t _width;
    size_t _height;
    char *_data;
    size_t _g;
    FILE *_raw;
};

#endif
