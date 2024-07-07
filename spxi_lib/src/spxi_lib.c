#include "spxi_lib.h"
#include <fileapi.h>
#include <errhandlingapi.h>
#include <handleapi.h>

// Hack to get around needing more windows headers
#define W_ERROR_FILE_EXISTS 80

int suffixCheck(const char *path) {
    const int pathLen = strlen(path);
    const char suffix[6] = { path[pathLen-5], path[pathLen-4], path[pathLen-3], path[pathLen-2], path[pathLen-1] };

    const int check = strcmp(suffix, ".spxi");
    if (check != 0) {
        return INVALID_SUFFIX;
    }

    return 0;
}

int spxiWrite(const char *path, char flags) {
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

    CloseHandle(fileHandle);
    return 0;
}