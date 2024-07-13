#include <fileapi.h>
#include <errhandlingapi.h>
#include <handleapi.h>

#include "spxi_lib.h"

// Hack to get around needing more windows headers
#define W_ERROR_FILE_EXISTS 80
#define ERROR_IO_PENDING 997L

#define SPACER_3_BYTES 0x0, 0x0, 0x0

int suffixCheck(const char *path) {
    const int pathLen = strlen(path);
    const char suffix[6] = { path[pathLen-5], path[pathLen-4], path[pathLen-3], path[pathLen-2], path[pathLen-1] };

    const int check = strcmp(suffix, ".spxi");
    if (check != 0) {
        return INVALID_SUFFIX;
    }

    return 0;
}

typedef struct _ColorIDNode {
    RGBA_Color color;
    unsigned int ID;
    struct _ColorIDNode *next;
} ColorIDNode;

typedef struct _LLHead {
    ColorIDNode *next;
    size_t size;
    ColorIDNode *last;
} LLHead;

#define LLHEAD_INIT (LLHead){NULL, 0, NULL};

int appendCIDNode(LLHead *list, RGBA_Color color, unsigned int ID) {
    list->size++;

    // Check that the list might need initialization and if so fill
    if (list->next == NULL){
        list->next = malloc(sizeof(ColorIDNode));
        list->last = list->next;

        list->next->color = color;
        list->next->ID = ID;
        list->next->next = NULL;

        return 0;
    }
    
    ColorIDNode *curNode = list->last;

    curNode->next = malloc(sizeof(ColorIDNode));
    if (curNode->next == NULL) {
        return ALLOCATION_FAILED;
    }

    curNode->next->color = color;
    curNode->next->ID = ID;
    curNode->next->next = NULL;

    list->last = curNode->next;

    return 0;
}

// Returns 0 if none present, returns 1 if present
int findCID(LLHead *list, RGBA_Color color) {
    if (list->next != NULL){
        ColorIDNode *curNode = list->next;
        int iter = 0;
        do {
            if (curNode->color.red   == color.red   &&
                curNode->color.green == color.green &&
                curNode->color.blue  == color.blue  &&
                curNode->color.alpha == color.alpha
            ) {
                return iter;
            }

            iter++;
            curNode = curNode->next;
        } while (curNode != NULL) ;
    }

    return -1;
}

int getColorFromIdx(LLHead *list, int idx, RGBA_Color *destInfo) {
    if (list->next != NULL) {
        ColorIDNode *curNode = list->next;

        for (size_t i = 0; i < idx; i++) {
            curNode = curNode->next;
        }

        *destInfo = curNode->color;
        return 0;
    }

    return 1; // Not present
}

int destroyList(LLHead *list) {
    ColorIDNode *curTarget = list->next;

    if (curTarget == NULL) {
        return 0;
    }

    ColorIDNode *nextTarget;
    while (curTarget->next != NULL) {
        nextTarget = curTarget->next;
        free(curTarget);
        curTarget = nextTarget;
    }
    
    free(curTarget);

    *list = LLHEAD_INIT;

    return 0;
}

int spxiWrite(const char *path, char flags, SPXIHeader header, RGBA_Color *pixels, unsigned int numPixels) {
    if (suffixCheck(path) == INVALID_SUFFIX) {
        return INVALID_SUFFIX;
    }

    //Should an existing file be overwritten or throw errors
    DWORD creationRuleset = 1;
    if (flags ^ OVERWRITE_OK == 0) {
        creationRuleset = 2; // Always create a new file, overriding the old
    }
    
    HANDLE fileHandle = CreateFileA(path, GENERIC_WRITE, 0, NULL, creationRuleset, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        if (err == W_ERROR_FILE_EXISTS)
        {
            return FILE_EXISTS;
        }
        else {
            return GENERIC_WINDOWS_ERR; // TODO: Get more specific with the error
        }
    }

    // Compose the header into one block to be written to the file

    BYTE headerBuf[35] = {
        0x73, 0x70, 0x78, 0x69, // Magic number
        header.version,
        35, SPACER_3_BYTES,                 //FIXME: Calculate file size for real!!!
        header.width,    SPACER_3_BYTES,
        header.height,   SPACER_3_BYTES,
        header.BPP };
    // There must be a better way to do this, but I'm not in the mood to fix it
    memcpy_s(&headerBuf[18], 38, &header.bitmask.red, 4); 
    memcpy_s(&headerBuf[22], 38, &header.bitmask.green, 4);
    memcpy_s(&headerBuf[26], 38, &header.bitmask.blue, 4);
    memcpy_s(&headerBuf[30], 38, &header.bitmask.alpha, 4);
    headerBuf[34] = header.flags;

    WINBOOL writeErr = WriteFile(fileHandle, &headerBuf, 35, NULL, NULL);
    if (writeErr == 0) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return GENERIC_WINDOWS_ERR;
        }
    }
    
    LLHead idList = LLHEAD_INIT;

    int IDDepth = 1;
    if (flags & ID_DEPTH == 1) { // Depth is 2 byte
        IDDepth = 2;
    }

    BYTE *pxIdxs = malloc(numPixels * IDDepth);

    // Write the Color ID Section
    int IDTicker = 0;
    for (size_t i = 0; i < numPixels; i++) {
        Pixel targetPixel = pixels[i];

        int searchedCID = findCID(&idList, targetPixel);
        if (searchedCID == -1) {
            appendCIDNode(&idList, targetPixel, IDTicker);
            pxIdxs[i] = IDTicker;
            IDTicker++;
            continue;
        }

        pxIdxs[i] = searchedCID;
    }

    
    int CIDBufSize = (IDTicker * (IDDepth + header.BPP)) + IDDepth;
    BYTE *CIDBuf = malloc(CIDBufSize);
    memset(CIDBuf, 0, CIDBufSize); // Zero the memory

    // Write the number of color - ID pairs
    memset(CIDBuf, IDTicker, IDDepth);
    
    int bppDivisior = 3;
    if (header.BPP == 32) {
        bppDivisior = 4;
    }

    // Calculate and configure write to file for the color ID definitions
    for (size_t i = 0; i < IDTicker; i++) {
        memset(&CIDBuf[(IDDepth*(i+1)) + ((header.BPP/8)*i)], i, IDDepth);

        RGBA_Color tickerColor;
        getColorFromIdx(&idList, i, &tickerColor);
        memset(&CIDBuf[(IDDepth*(i+2)) + ((header.BPP/8)*i)], tickerColor.red, 1); //FIXME: Not actually accounting for ID
        memset(&CIDBuf[(IDDepth*(i+2)) + ((header.BPP/8)*i) + 1], tickerColor.green, 1);
        memset(&CIDBuf[(IDDepth*(i+2)) + ((header.BPP/8)*i) + 2], tickerColor.blue, 1);
        if (bppDivisior = 4) {
            memset(&CIDBuf[(IDDepth*(i+2)) + ((header.BPP/8)*i) + 3], tickerColor.blue, 1);
        }
    }

    writeErr = WriteFile(fileHandle, CIDBuf, (IDTicker * (IDDepth + (header.BPP/8))) + IDDepth, NULL, NULL);
    if (writeErr == 0) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return GENERIC_WINDOWS_ERR;
        }
    }
    destroyList(&idList); // TODO: Take into Visual Studio and double confirm no memory leak

    writeErr = WriteFile(fileHandle, pxIdxs, numPixels * IDDepth, NULL, NULL);
    if (writeErr == 0) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return GENERIC_WINDOWS_ERR;
        }
    }
    free(pxIdxs);

    CloseHandle(fileHandle);
    return 0;
}

int spxiRead(const char *target, SPXIHeader *fileInfoDest, Pixel *pixelInfoDest) {
    return SUCCESSFUL_READ;
}