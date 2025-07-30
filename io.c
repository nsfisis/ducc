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
    char* buf = calloc(1024 * 1024, sizeof(char));
    char* cur = buf;
    char* tmp = calloc(1024, sizeof(char));
    while (fgets(tmp, 1024, in)) {
        size_t len = strlen(tmp);
        memcpy(cur, tmp, len);
        cur += len;
    }
    fclose(in);

    InFile* in_file = calloc(1, sizeof(InFile));
    in_file->filename = filename;
    in_file->buf = buf;
    return in_file;
}
