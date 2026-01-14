#ifndef DUCC_PREPROCESS_H
#define DUCC_PREPROCESS_H

#include "common.h"
#include "io.h"
#include "token.h"

TokenArray* preprocess(InFile* src, StrArray* included_files, StrArray* user_include_dirs, StrArray* user_defines,
                       bool generate_system_deps, bool generate_user_deps);
void concat_adjacent_string_literals(TokenArray* pp_tokens);
void print_token_to_file(FILE* output_file, TokenArray* pp_tokens);

#endif
