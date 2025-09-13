#ifndef DUCC_STD_H
#define DUCC_STD_H

#include <stddef.h>

int atoi(const char*);
void* calloc(size_t, size_t);
void exit(int);
int mkstemps(char*, int);
void* realloc(void*, size_t);
long strtol(const char*, char**, int);
int system(const char*);

#define assert(x) \
    do { \
        if (!(x)) { \
            fatal_error("%s:%d: assertion failed", __FILE__, __LINE__); \
        } \
    } while (0)

#define F_OK 0
#define R_OK 4
int access(const char*, int);

#define PATH_MAX 4096

char* dirname(char*);

#endif
