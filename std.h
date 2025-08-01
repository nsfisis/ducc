#include <stddef.h>

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
void* memset(void*, int, size_t);
int printf(const char*, ...);
int sprintf(char*, const char*, ...);
int strcmp(const char*, const char*);
size_t strlen(const char*);
int strncmp(const char*, const char*, size_t);
char* strstr(const char*, const char*);

#include <stdarg.h>

int vfprintf(FILE*, const char*, va_list);

#define F_OK 0
#define R_OK 4
int access(const char*, int);

#define PATH_MAX 4096

typedef long ssize_t;
ssize_t readlink(const char*, char*, size_t);
char* dirname(char*);
