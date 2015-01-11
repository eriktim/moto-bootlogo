#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string.h>

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
    for (map<string, BinHeader*>::const_iterator it = _headers.begin(),
            end = _headers.end(); it != end; it++) {
        BinHeader *header = it->second;
        delete header;
    }
}

bool BinFile::copy_image_header(string fromTag, string toTag)
{
    if (_count < 0) {
        _parse_header();
    }

    BinHeader *from = _headers[fromTag];
    if (!from) {
        cerr << "Image header not found: " << fromTag << endl;
        return false;
    }

    BinHeader *to = _headers[toTag];
    if (!to) {
        cerr << "Image header not found: " << toTag << endl;
        return false;
    }

    size_t offset = from->get_offset();
    size_t length = from->get_size();
    size_t g = to->get_g();

    _file.close();
    _file.open(_filename.c_str(), ios::in|ios::out|ios::binary);
    _file.seekg(g + 24, ios::beg);

    // convert offset|length to binary
    char data[8];
    for (int i = 0; i < 4; ++i) {
        data[i] = (offset >> 8 * i) & 0xff;
    }
    for (int i = 0; i < 4; ++i) {
        data[4 + i] = (length >> 8 * i) & 0xff;
    }

    // update image header
    _file.write(data, 8);
    _file.close();

    // file should be re-parsed
    _count = -1;

    return true;
}

map<string, BinHeader*> BinFile::get_headers(void)
{
    if (_count < 0) {
        _parse_header();
    }
    return _headers;
}

bool BinFile::replace_image(string tag, string filename)
{
    if (_count < 0) {
        _parse_header();
    }

    // find header with tag
    BinHeader *header = _headers[tag];
    if (!header) {
        cerr << "Image header not found: " << tag << endl;
        return false;
    }

    // replace the data in the BinImage
    BinImage *image = header->get_image();
    if (!image->set_data_from_png(filename)) {
        cerr << "Failed reading image data from '" << filename << "'" << endl;
        return false;
    }

    if (_file.is_open()) {
        _file.close();
    }
    _file.open(_filename.c_str(), ios::in|ios::out|ios::binary);
    if (_file.fail()) {
        cerr << "Failed opening '" << _file << "' for writing" << endl;
        return false;
    }

    size_t length = image->get_size();

    size_t gHeader = header->get_g();
    char size[2];
    size[0] = (length >> 8) & 0xff;
    size[1] = length & 0xff;

    _file.seekg(gHeader + 24 + 4, ios::beg);
    _file.write(size, 2);

    size_t gImage = header->get_offset();
    char *data = (char *)image->get_data();

    _file.seekg(gImage + 8 + 4, ios::beg);
    _file.write(data, length);

    // file should be re-parsed
    _count = -1;

    return true;
}

size_t BinFile::_read_value(void) {
    char bytes[4];
    _file.read(bytes, 4);
    size_t value = 0;
    for (int i=3; i>=0; i--) {
        value += (uint8_t)bytes[i] << 8 * i;
    }
    return value;
}

/*
 * Example header:
 * 4d 6f 74 6f 4c 6f 67 6f-00 6d 00 00 00 6c 6f 67
 * 6f 5f 62 6f 6f 74 00 00-00 00 00 00 00 00 00 00
 * 00 00 00 00 00 00 02 00-00 a5 20 07 00 6c 6f 67
 * 6f 5f 62 61 74 74 65 72-79 00 00 00 00 00 00 00
 * 00 00 00 00 00 00 24 07-00 04 33 00 00 6c 6f 67
 * 6f 5f 75 6e 6c 6f 63 6b-65 64 00 00 00 00 00 00
 * 00 00 00 00 00 00 58 07-00 77 77 00 00 ff ff ff
 * ff ff ff ff ff ff ff ff-ff ff ff ff ff ff ff ff
 */
void BinFile::_parse_header(void)
{
    _headers.clear();

    if (_file.is_open()) {
        _file.close();
    }

    _file.open(_filename.c_str(), ios::in|ios::binary);

    if (_file.fail()) {
        cerr << "Failed opening file '" << _filename << "'." << endl;
        return;
    }

    char bytes[16];
    _file.seekg(0, ios::beg);

    cout << "Parsing header of '" << _filename << "'." << endl;

    // header must start with 'MotoLogo'
    _file.read(bytes, 9);
    if (_file.fail() || strcmp(bytes, "MotoLogo")) {
        cerr << "Header is NOT valid" << endl;
        return;
    }

    // next byte contains the header size
    _file.read(bytes, 2);
    uint8_t size = (uint8_t)bytes[0];
    if (_file.fail() || !size) {
        cerr << "Header is empty" << endl;
        return;
    }
    cout << "Header size: " << (int)size << " bytes." << endl;

    // move to first logo
    _file.seekg(2, ios::cur);

    while (_file.tellg() < size) {
        // read tag
        _file.read(bytes, 24);
        string tag(bytes);

        // read offset and length
        size_t offset = _read_value();
        size_t length = _read_value();

        // print results
        cout << "Header logo: '" << tag << "', starts at byte " << offset
                << " and occupies " << length << " bytes." << endl;

        // check if the result is OK
        if (tag.empty() || !offset || !length) {
            cerr << "Found an invalid image header" << endl;
            continue;
        }

        // get data
        char data[length];
        int g = _file.tellg();
        _file.seekg(offset, ios::beg);
        _file.read(data, length);
        _file.seekg(g, ios::beg);

        // create objects
        BinImage *image = new BinImage(data, length);
        BinHeader *header = new BinHeader(offset, length, g - 32, image);

        // keep a reference to the image header
        _headers.insert(pair<string, BinHeader*>(tag, header));
    }

    _count = _headers.size();

}
