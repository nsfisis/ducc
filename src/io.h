#ifndef DUCC_IO_H
#define DUCC_IO_H

#include "json.h"
#include "std.h"

struct SourceLocation {
    const char* filename;
    int line;
};
typedef struct SourceLocation SourceLocation;

void sourcelocation_build_json(JsonBuilder* builder, SourceLocation* loc);

struct InFile {
    const char* buf;
    int pos;
    SourceLocation loc;
};
typedef struct InFile InFile;

InFile* infile_open(const char* filename);
BOOL infile_eof(InFile* f);
char infile_peek_char(InFile* f);
char infile_next_char(InFile* f);
BOOL infile_consume_if(InFile* f, char expected);

#endif
