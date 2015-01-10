#ifndef BINFILE_H
#define BINFILE_H

#include <fstream>
#include <map>
#include <string>

#include "BinHeader.h"
#include "BinImage.h"

using namespace std;

class BinHeader;

class BinFile
{
public:
    BinFile(string filename);
    ~BinFile();
    bool copy_image_header(string fromTag, string toTag);
    map<string, BinHeader*> get_headers(void);
    bool replace_image(string tag, string filename);

private:
    size_t _read_value(void);
    void _parse_header(void);

    string _filename;
    fstream _file;
    map<string, BinHeader*> _headers;
    int _count;
};

#endif
