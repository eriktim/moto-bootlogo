
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
    vector<BinImage*> images = file->get_images();
    int size = images.size();
    cout << "Found " << size << " image" << (size == 1 ? "" : "s") << "." << endl;
    for (vector<BinImage*>::const_iterator it = images.begin(),
            end = images.end(); it != end; it++) {
        BinImage *image = *it;
        image->create_png();
    }
    return 0;
}
