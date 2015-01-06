#include <iostream>

#include "BinFile.h"

BinFile::BinFile(string filename)
{
    _filename = filename;
    _count = -1;
}

BinFile::~BinFile()
{
    if (_file.is_open()) {
        _file.close();
    }
}

list<BinImage> BinFile::get_images(void)
{
    if (_count < 0) {
        _parse_headers();
    }
    return list<BinImage>();
}

void BinFile::replace_image(string type, string filename)
{

}

void BinFile::reset_unlocked_logo(void)
{

}

void BinFile::_parse_headers(void)
{
    if (_file.is_open()) {
        _file.close();
    }

    _file.open(_filename.c_str(), ios::in|ios::binary);

    if (_file.fail()) {
        cerr << "Failed opening file '" << _filename << "'." << endl;
        return;
    }

    cout << "Parsing headers of '" << _filename << "'." << endl;

    
}
