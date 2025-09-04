#ifndef DUCC_STD_H
#define DUCC_STD_H

#include <stddef.h>

int atoi(const char*);
void* calloc(size_t, size_t);
void exit(int);
int getchar();
int isalnum(int);
int isalpha(int);
int isdigit(int);
int isspace(int);
void* memcpy(void*, const void*, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
int mkstemps(char*, int);
void* realloc(void*, size_t);
int strcmp(const char*, const char*);
size_t strlen(const char*);
int strncmp(const char*, const char*, size_t);
char* strdup(const char*);
char* strndup(const char*, size_t);
char* strstr(const char*, const char*);
char* strrchr(const char*, int);
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
