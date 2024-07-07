#include "spxi_lib.h"

int main(void) {
    SPXIHeader header = {};
    header.version = 0x0;
    header.fileSize = sizeof(SPXIHeader);
    header.width = 1;
    header.height = 2;
    header.BPP = 32;
    header.bitmask = DEFAULT_BITMASK;
    header.flags = ID_DEPTH;

    spxiWrite("test.spxi", OVERWRITE_OK, header);

    return 0;
}