#include "BinFile.h"

BinFile::BinFile(string filename)
{
    _filename = filename;
    _count = -1;
}

list<BinImage> BinFile::getImages(void)
{
    return list<BinImage>();
}

void BinFile::replaceImage(string type, string filename)
{

}

void BinFile::resetUnlockedLogo(void)
{

}

