#ifndef DUCC_PREPROCESS_H
#define DUCC_PREPROCESS_H

#include <stdio.h>
#include "common.h"
#include "io.h"
#include "token.h"

TokenArray* preprocess(InFile* src, StrArray* included_files, StrArray* user_include_dirs);
void print_token_to_file(FILE* output_file, TokenArray* pp_tokens);

#endif
