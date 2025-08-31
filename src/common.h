#ifndef DUCC_COMMON_H
#define DUCC_COMMON_H

#include "std.h"

_Noreturn void fatal_error(const char* msg, ...);

#define unreachable() fatal_error("%s:%d: unreachable", __FILE__, __LINE__)
#define unimplemented() fatal_error("%s:%d: unimplemented", __FILE__, __LINE__)

BOOL str_ends_with(const char* s, const char* suffix);

struct StrBuilder {
    size_t len;
    size_t capacity;
    char* buf;
};
typedef struct StrBuilder StrBuilder;

void strbuilder_init(StrBuilder* b);
// `size` must include a trailing null byte.
void strbuilder_reserve(StrBuilder* b, size_t size);
void strbuilder_append_char(StrBuilder* b, int c);
void strbuilder_append_string(StrBuilder* b, const char* s);

struct StrArray {
    size_t len;
    size_t capacity;
    const char** data;
};
typedef struct StrArray StrArray;

void strings_init(StrArray* strings);
void strings_reserve(StrArray* strings, size_t size);
int strings_push(StrArray* strings, const char* str);

#endif
