#ifndef BINFILE_H
#define BINFILE_H

#include <fstream>
#include <list>
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
    list<BinImage> get_images(void);
    void replace_image(string type, string filename);
    void reset_unlocked_logo(void);

private:
    void _parse_headers(void);

    std::string _filename;
    fstream _file;
    list<BinHeader> _headers;
    int _count;
};

#endif
