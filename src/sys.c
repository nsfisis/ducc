#include "sys.h"
#include <libgen.h>
#include <linux/limits.h>
#include <stdlib.h>
#include "std.h"

// TODO: #inclue some header.
#ifndef __ducc__
typedef long ssize_t;
#endif
ssize_t readlink(const char*, char*, size_t);

static char* get_self_path() {
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
