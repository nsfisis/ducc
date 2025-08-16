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
