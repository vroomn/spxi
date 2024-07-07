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

int spxiWrite(const char *path, char flags, SPXIHeader header) {
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
        header.fileSize, SPACER_3_BYTES,
        header.width,    SPACER_3_BYTES,
        header.height,   SPACER_3_BYTES,
        header.BPP };
    // There must be a better way to do this, but I'm not in the mood to fix it
    memcpy_s(&headerBuf[18], 38, &header.bitmask.red, 4); 
    memcpy_s(&headerBuf[22], 38, &header.bitmask.green, 4);
    memcpy_s(&headerBuf[26], 38, &header.bitmask.blue, 4);
    memcpy_s(&headerBuf[30], 38, &header.bitmask.alpha, 4);
    headerBuf[34] = header.flags;

    ULONG bytesWritten = 0;
    WINBOOL writeErr = WriteFile(fileHandle, &headerBuf, 35, NULL, NULL);
    if (writeErr == 0) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return GENERIC_WINDOWS_ERR;
        }
    }
    

    CloseHandle(fileHandle);
    return 0;
}