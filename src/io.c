#include "io.h"
#include "common.h"
#include "json.h"

void sourcelocation_build_json(JsonBuilder* builder, SourceLocation* loc) {
    jsonbuilder_object_start(builder);
    jsonbuilder_object_member_start(builder, "filename");
    jsonbuilder_string(builder, loc->filename);
    jsonbuilder_object_member_end(builder);
    jsonbuilder_object_member_start(builder, "line");
    jsonbuilder_integer(builder, loc->line);
    jsonbuilder_object_member_end(builder);
    jsonbuilder_object_end(builder);
}

InFile* infile_open(const char* filename) {
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
    in_file->buf = buf;
    in_file->loc.filename = filename;
    in_file->loc.line = 1;
    return in_file;
}

BOOL infile_eof(InFile* f) {
    return f->buf[f->pos] == '\0';
}

char infile_peek_char(InFile* f) {
    char c = f->buf[f->pos];

    // Skip a pair of backslash and new-line.
    if (c == '\\') {
        char c2 = f->buf[f->pos + 1];
        // C23: 5.1.1.2
        // A source file that is not empty shall end in a new-line character, which shall not be immediately preceded by
        // a backslash character before any such splicing takes place.
        if (c2 == '\0') {
            fatal_error("%s:%d: <new-line> expected, but got <eof>", f->loc.filename, f->loc.line);
        }
        // TODO: crlf
        if (c2 == '\r' || c2 == '\n') {
            f->pos += 2;
            ++f->loc.line;
            return infile_peek_char(f);
        }
    }

    // Normalize new-line.
    // TODO: crlf
    if (c == '\r')
        c = '\n';
    return c;
}

char infile_next_char(InFile* f) {
    char c = infile_peek_char(f);
    ++f->pos;
    if (c == '\n')
        ++f->loc.line;
    return c;
}

BOOL infile_consume_if(InFile* f, char expected) {
    if (infile_peek_char(f) == expected) {
        infile_next_char(f);
        return TRUE;
    } else {
        return FALSE;
    }
}
