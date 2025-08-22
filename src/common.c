void fatal_error(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
}

#define unreachable() fatal_error("%s:%d: unreachable", __FILE__, __LINE__)

#define unimplemented() fatal_error("%s:%d: unimplemented", __FILE__, __LINE__)

struct StrBuilder {
    size_t len;
    size_t capacity;
    char* buf;
};
typedef struct StrBuilder StrBuilder;

void strbuilder_init(StrBuilder* b) {
    b->len = 0;
    b->capacity = 16;
    b->buf = calloc(b->capacity, sizeof(char));
}

// `size` must include a trailing null byte.
void strbuilder_reserve(StrBuilder* b, size_t size) {
    if (size <= b->capacity)
        return;
    b->capacity *= 2;
    b->buf = realloc(b->buf, b->capacity * sizeof(char));
    memset(b->buf + b->len, 0, (b->capacity - b->len) * sizeof(char));
}

void strbuilder_append_char(StrBuilder* b, int c) {
    strbuilder_reserve(b, b->len + 1 + 1);
    b->buf[b->len++] = c;
}

struct StrArray {
    size_t len;
    size_t capacity;
    const char** data;
};
typedef struct StrArray StrArray;

void strings_init(StrArray* strings) {
    strings->len = 0;
    strings->capacity = 32;
    strings->data = calloc(strings->capacity, sizeof(const char*));
}

void strings_reserve(StrArray* strings, size_t size) {
    if (size <= strings->capacity)
        return;
    strings->capacity *= 2;
    strings->data = realloc(strings->data, strings->capacity * sizeof(const char*));
    memset(strings->data + strings->len, 0, (strings->capacity - strings->len) * sizeof(const char*));
}

int strings_push(StrArray* strings, const char* str) {
    strings_reserve(strings, strings->len + 1);
    strings->data[strings->len] = str;
    return ++strings->len;
}
