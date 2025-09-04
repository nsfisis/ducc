#ifndef DUCC_CODEGEN_H
#define DUCC_CODEGEN_H

#include <stdio.h>
#include "ast.h"

void codegen(Program* prog, FILE* out);

#endif
