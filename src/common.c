#include "common.h"
#include <stdarg.h>
#include <stdio.h>

void fatal_error(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
}

bool str_ends_with(const char* s, const char* suffix) {
    size_t l1 = strlen(s);
    size_t l2 = strlen(suffix);
    if (l1 < l2)
        return false;
    return strcmp(s + l1 - l2, suffix) == 0;
}

void strbuilder_init(StrBuilder* b) {
    b->len = 0;
    b->capacity = 16;
    b->buf = calloc(b->capacity, sizeof(char));
}

// `size` must include a trailing null byte.
void strbuilder_reserve(StrBuilder* b, size_t size) {
    if (size <= b->capacity)
        return;
    while (b->capacity < size) {
        b->capacity *= 2;
    }
    b->buf = realloc(b->buf, b->capacity * sizeof(char));
    memset(b->buf + b->len, 0, (b->capacity - b->len) * sizeof(char));
}

void strbuilder_append_char(StrBuilder* b, int c) {
    strbuilder_reserve(b, b->len + 1 + 1);
    b->buf[b->len++] = c;
}

void strbuilder_append_string(StrBuilder* b, const char* s) {
    int len = strlen(s);
    strbuilder_reserve(b, b->len + len + 1);
    for (int i = 0; i < len; ++i) {
        b->buf[b->len++] = s[i];
    }
}

void strings_init(StrArray* strings) {
    strings->len = 0;
    strings->capacity = 32;
    strings->data = calloc(strings->capacity, sizeof(const char*));
}

void strings_reserve(StrArray* strings, size_t size) {
    if (size <= strings->capacity)
        return;
    while (strings->capacity < size) {
        strings->capacity *= 2;
    }
    strings->data = realloc(strings->data, strings->capacity * sizeof(const char*));
    memset(strings->data + strings->len, 0, (strings->capacity - strings->len) * sizeof(const char*));
}

int strings_push(StrArray* strings, const char* str) {
    strings_reserve(strings, strings->len + 1);
    strings->data[strings->len] = str;
    return ++strings->len;
}

void strings_pop(StrArray* strings) {
    if (strings->len > 0) {
        strings->len--;
    }
}
