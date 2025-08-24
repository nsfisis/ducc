#ifndef DUCC_PARSE_H
#define DUCC_PARSE_H

#include "ast.h"
#include "preprocess.h"

Program* parse(TokenArray* tokens);
BOOL pp_eval_constant_expression(TokenArray* pp_tokens);

#endif
