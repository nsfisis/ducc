char* get_self_path() {
    char* buf = calloc(PATH_MAX, sizeof(char));
    ssize_t len = readlink("/proc/self/exe", buf, PATH_MAX - 1);
    if (len == -1 || len == PATH_MAX - 1) {
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

// It returns a path not including final / except for root directory.
char* get_self_dir() {
    char* path = get_self_path();
    return dirname(path);
}
