#ifndef SPXI_LIB
#define SPXI_LIB

#define GENERIC_WINDOWS_ERR -1
#define FILE_EXISTS -2
#define INVALID_SUFFIX -3

/* Specifies byte flags for controlling write logic*/
// Allows the program to overwrite existing files
#define OVERWRITE_OK 0x1

/* Currently only writes to unique files, not to existing ones 
   TODO: Write system to override files if present w/ different func*/
extern int spxiWrite(const char *path, char flags);

#endif