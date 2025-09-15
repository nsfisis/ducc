#ifndef DUCC_IO_H
#define DUCC_IO_H

#include "json.h"

typedef struct {
    const char* filename;
    int line;
} SourceLocation;

void sourcelocation_build_json(JsonBuilder* builder, SourceLocation* loc);

typedef struct {
    const char* buf;
    int pos;
    SourceLocation loc;
} InFile;

InFile* infile_open(const char* filename);
bool infile_eof(InFile* f);
char infile_peek_char(InFile* f);
char infile_next_char(InFile* f);
bool infile_consume_if(InFile* f, char expected);

#endif
