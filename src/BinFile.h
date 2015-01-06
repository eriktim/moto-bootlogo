#ifndef BINFILE_H
#define BINFILE_H

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
    list<BinImage> getImages(void);
    void replaceImage(string type, string filename);
    void resetUnlockedLogo(void);

private:
    std::string _filename;
    list<BinHeader> _headers;
    int _count;
};

#endif
