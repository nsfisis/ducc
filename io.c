struct InFile {
    const char* filename;
    char* buf;
};
typedef struct InFile InFile;

InFile* read_all(const char* filename) {
    FILE* in;
    if (strcmp(filename, "-") == 0) {
        in = stdin;
    } else {
        in = fopen(filename, "rb");
    }
    if (!in) {
        return NULL;
    }

    size_t buf_size = 1024 * 10;
    char* buf = calloc(buf_size, sizeof(char));
    char* cur = buf;
    char* tmp = calloc(1024, sizeof(char));

    while (fgets(tmp, 1024, in)) {
        size_t len = strlen(tmp);
        size_t used_size = cur - buf;

        if (buf_size <= used_size + len) {
            size_t old_size = buf_size;
            buf_size *= 2;
            buf = realloc(buf, buf_size);
            memset(buf + old_size, 0, buf_size - old_size);
            cur = buf + used_size;
        }

        memcpy(cur, tmp, len);
        cur += len;
    }
    fclose(in);

    InFile* in_file = calloc(1, sizeof(InFile));
    in_file->filename = filename;
    in_file->buf = buf;
    return in_file;
}
