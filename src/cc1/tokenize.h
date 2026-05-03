#ifndef DUCC_TOKENIZE_H
#define DUCC_TOKENIZE_H

#include "io.h"
#include "token.h"

TokenArray* tokenize(InFile* src);
TokenArray* convert_pp_tokens_to_tokens(TokenArray* pp_tokens);

#endif
