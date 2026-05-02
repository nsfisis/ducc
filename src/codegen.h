#ifndef DUCC_CODEGEN_H
#define DUCC_CODEGEN_H

#include "ast.h"

void codegen(Program* prog, const char* input_filename, FILE* out);

#endif
