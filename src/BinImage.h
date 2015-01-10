#ifndef BINIMAGE_H
#define BINIMAGE_H

#include <stdint.h>

using namespace std;

class BinImage
{
public:
    BinImage(char *data, size_t size);
    ~BinImage();
    void create_png(string filename);
    bool set_data_from_png(string filename);

private:
    void _decode(void);
    void _encode(void);
    size_t _read_dimension(void);

    size_t _size;
    size_t _width;
    size_t _height;
    uint8_t *_data;
    size_t _g;
    uint8_t *_raw;
};

#endif
