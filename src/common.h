#ifndef DUCC_COMMON_H
#define DUCC_COMMON_H

#include "ducc.h"

_Noreturn void fatal_error(const char* msg, ...);

// TODO
#ifdef __ducc__
#define unreachable() fatal_error("%s:%d: unreachable", __FILE__, __LINE__)
#endif

#define unimplemented() fatal_error("%s:%d: unimplemented", __FILE__, __LINE__)

bool str_ends_with(const char* s, const char* suffix);

typedef struct {
    size_t len;
    size_t capacity;
    char* buf;
} StrBuilder;

void strbuilder_init(StrBuilder* b);
// `size` must include a trailing null byte.
void strbuilder_reserve(StrBuilder* b, size_t size);
void strbuilder_append_char(StrBuilder* b, int c);
void strbuilder_append_string(StrBuilder* b, const char* s);

typedef struct {
    size_t len;
    size_t capacity;
    const char** data;
} StrArray;

void strings_init(StrArray* strings);
void strings_reserve(StrArray* strings, size_t size);
int strings_push(StrArray* strings, const char* str);
void strings_pop(StrArray* strings);

#endif
