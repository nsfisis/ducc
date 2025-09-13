#include "fs.h"
#include <stdlib.h>
#include <string.h>
#include "std.h"

// 'ext' must include '.'.
char* replace_extension(const char* file_name, const char* ext) {
    size_t len = strlen(file_name);
    const char* last_slash = strrchr(file_name, '/');
    const char* last_dot = strrchr(file_name, '.');

    size_t ext_len = strlen(ext);
    size_t base_len;
    // !last_slash: foo.c
    // last_slash < last_dot: ./bar/foo.c
    if (last_dot && (!last_slash || last_slash < last_dot)) {
        base_len = last_dot - file_name;
    } else {
        base_len = len;
    }

    char* result = calloc(base_len + ext_len + 1, sizeof(char));
    memcpy(result, file_name, base_len);
    memcpy(result + base_len, ext, ext_len);
    result[base_len + ext_len] = '\0';

    return result;
}
