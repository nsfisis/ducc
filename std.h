typedef long size_t;

struct FILE;
typedef struct FILE FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int atoi(const char*);
void* calloc(size_t, size_t);
void exit(int);
int fclose(FILE*);
int fprintf(FILE*, const char*, ...);
char* fgets(char*, int, FILE*);
FILE* fopen(const char*, const char*);
int getchar(void);
int isalnum(int);
int isalpha(int);
int isdigit(int);
int isspace(int);
void* memcpy(void*, void*, size_t);
void* memmove(void*, void*, size_t);
int printf(const char*, ...);
int sprintf(char*, const char*, ...);
int strcmp(const char*, const char*);
size_t strlen(const char*);
int strncmp(const char*, const char*, size_t);
char* strstr(const char*, const char*);

#define NULL 0

#ifdef __ducc__

// System V Application Binary Interface
// AMD64 Architecture Processor Supplement
// (With LP64 and ILP32 Programming Models)
// Version 1.0
// Figure 3.34: va_list Type Declaration
struct __va_list__ {
    // unsigned int gp_offset;
    // unsigned int fp_offset;
    int gp_offset;
    int fp_offset;
    void* overflow_arg_area;
    void* reg_save_area;
};
// ducc currently does not support array type.
// typedef struct __va_list__ va_list[1];
typedef struct __va_list__* va_list;

// va_start() is currently implemented as a special form due to the limitation of #define macro.
void va_end(va_list args) {
}

#else

#include <stdarg.h>

#endif

int vfprintf(FILE*, const char*, va_list);
