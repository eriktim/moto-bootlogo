#ifndef BINFILE_H
#define BINFILE_H

#include <fstream>
#include <vector>
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
    vector<BinImage*> get_images(void);
    void replace_image(string type, string filename);
    void reset_unlocked_logo(void);

private:
    size_t _read_value(void);
    void _parse_header(void);

    string _filename;
    fstream _file;
    vector<BinHeader*> _headers;
    int _count;
};

#endif
