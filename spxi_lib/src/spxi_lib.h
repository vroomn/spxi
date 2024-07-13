#ifndef SPXI_LIB
#define SPXI_LIB

#define GENERIC_WINDOWS_ERR -1
#define FILE_EXISTS -2
#define INVALID_SUFFIX -3

// Linked list errors
#define ALLOCATION_FAILED -4

// Read-specific return codes
#define SUCCESSFUL_READ 0;

/* Specifies byte flags for controlling write logic*/
// Allows the program to overwrite existing files
#define OVERWRITE_OK 0x1

#if !defined(BYTE)
#define BYTE unsigned char
#endif

#if !defined(DWORD)
#define DWORD unsigned long long
#endif

typedef struct _RGBA_BITMASK
{
    DWORD red;
    DWORD green;
    DWORD blue;
    DWORD alpha;
} RGBA_BITMASK;

#define DEFAULT_BITMASK (RGBA_BITMASK){ \
    .red = 0xFF000000,    /* Red */     \
    .green = 0x00FF0000,  /* Green */   \
    .blue = 0x0000FF00,   /* Blue */    \
    .alpha = 0x000000FF,  /* Alpha */   \
};

typedef struct _SPXIHeader
{
    BYTE version;
    DWORD width;
    DWORD height;
    BYTE BPP;
    RGBA_BITMASK bitmask;
    BYTE flags;
} SPXIHeader;

#define RLE_ENABLED 0x1
#define ID_DEPTH 0x2

typedef struct _RGBA_Color {
    BYTE red;
    BYTE green;
    BYTE blue;
    BYTE alpha;
} RGBA_Color, Pixel;

/* Currently only writes to unique files, not to existing ones 
   TODO: Write system to override files if present w/ different func*/
extern int spxiWrite(const char *path, char flags, SPXIHeader header, RGBA_Color *pixels, unsigned int numPixels);

extern int spxiRead(const char *target, SPXIHeader *fileInfoDest, Pixel *pixelInfoDest);

#endif