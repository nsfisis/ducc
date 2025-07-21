void fatal_error(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
}

void unreachable() {
    fatal_error("unreachable");
}

char* read_all(FILE* in) {
    char* buf = calloc(1024 * 1024, sizeof(char));
    char* cur = buf;
    char* tmp = calloc(1024, sizeof(char));
    while (fgets(tmp, 1024, in)) {
        size_t len = strlen(tmp);
        memcpy(cur, tmp, len);
        cur += len;
    }
    return buf;
}

struct String {
    char* data;
    size_t len;
};
typedef struct String String;

char* string_to_cstr(const String* s) {
    char* buf = calloc(s->len + 1, sizeof(char));
    memcpy(buf, s->data, s->len);
    return buf;
}

int string_equals(const String* s1, const String* s2) {
    return s1->len == s2->len && strncmp(s1->data, s2->data, s1->len) == 0;
}

int string_equals_cstr(const String* s1, const char* s2) {
    size_t s2_len = strlen(s2);
    return s1->len == s2_len && strncmp(s1->data, s2, s1->len) == 0;
}
