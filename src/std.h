#ifndef DUCC_STD_H
#define DUCC_STD_H

#include <stddef.h>

// TODO: <stdlib.h>
int mkstemps(char*, int);

#define F_OK 0
#define R_OK 4
int access(const char*, int);

#define PATH_MAX 4096

char* dirname(char*);

#endif
