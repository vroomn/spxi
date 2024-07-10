#include "spxi_lib.h"

int main(void) {
    SPXIHeader header = {};
    header.version = 0x0;
    header.width = 1;
    header.height = 2;
    header.BPP = 24;
    header.bitmask = DEFAULT_BITMASK;
    header.flags = ID_DEPTH;

    //Pixel tmpColor = {255, 255, 255, 255};
    //Pixel testImg[] = { tmpColor, tmpColor, tmpColor };

    // 3x3 test image
    Pixel testImg[] = {
        (Pixel){255, 255, 255, 255}, (Pixel){255, 255, 255, 255}, (Pixel){255, 255, 255, 255},
        (Pixel){253, 255, 255, 255}, (Pixel){255, 253, 255, 255}, (Pixel){255, 255, 255, 255},
        (Pixel){255, 255, 255, 255}, (Pixel){255, 255, 255, 255}, (Pixel){255, 255, 255, 255}
    };

    spxiWrite("test.spxi", OVERWRITE_OK, header, testImg, sizeof(testImg) / sizeof(Pixel));

    return 0;
}