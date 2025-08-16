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

struct String {
    const char* data;
    size_t len;
};
typedef struct String String;

char* string_to_cstr(const String* s) {
    char* buf = calloc(s->len + 1, sizeof(char));
    memcpy(buf, s->data, s->len);
    return buf;
}

String* cstr_to_new_string(const char* cstr) {
    String* s = calloc(1, sizeof(String));
    s->len = strlen(cstr);
    s->data = strndup(cstr, s->len);
    return s;
}

BOOL string_equals(const String* s1, const String* s2) {
    return s1->len == s2->len && strncmp(s1->data, s2->data, s1->len) == 0;
}

BOOL string_equals_cstr(const String* s1, const char* s2) {
    size_t s2_len = strlen(s2);
    return s1->len == s2_len && strncmp(s1->data, s2, s1->len) == 0;
}
