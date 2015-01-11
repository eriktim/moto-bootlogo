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
    uint8_t *get_data(void);
    size_t get_size(void);
    bool set_data_from_png(string filename);

private:
    size_t _block_length(bool repeat, size_t g);
    void _decode(void);
    bool _encode(void);
    size_t _read_dimension(void);

    size_t _size;
    size_t _width;
    size_t _height;
    uint8_t *_data;
    size_t _g;
    uint8_t *_raw;
};

#endif
