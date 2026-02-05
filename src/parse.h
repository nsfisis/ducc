#ifndef DUCC_PARSE_H
#define DUCC_PARSE_H

#include "ast.h"
#include "common.h"
#include "preprocess.h"

Program* parse(TokenArray* tokens);
bool pp_eval_constant_expr(TokenArray* pp_tokens);
void eval_init_expr(StrBuilder* buf, AstNode* expr, Type* ty);

#endif
