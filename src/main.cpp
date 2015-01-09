
#include <iostream>
#include <stdlib.h>

#include "BinFile.h"

using namespace std;

void print_usage(void) {
    cout << "moto-bootlogo <logo.bin> [-i <newlogo.png>]" << endl;
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        print_usage();
        exit(1);
    }
    string filename = argv[1];
    BinFile *file = new BinFile(filename);
    map<string, BinHeader*> headers = file->get_headers();
    int size = headers.size();
    cout << "Found " << size << " image" << (size == 1 ? "" : "s") << "." << endl;
    for (map<string, BinHeader*>::const_iterator it = headers.begin(),
            end = headers.end(); it != end; ++it) {
        string tag = it->first;
        BinHeader *header = it->second;
        BinImage *image = header->get_image();
        image->create_png(tag.append(".png"));
    }
    delete file;
    return 0;
}
