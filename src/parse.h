#ifndef DUCC_PARSE_H
#define DUCC_PARSE_H

#include "ast.h"
#include "common.h"
#include "preprocess.h"

Program* parse(TokenArray* tokens);
bool pp_eval_constant_expr(TokenArray* pp_tokens);

typedef enum {
    InitDataBlockKind_addr,
    InitDataBlockKind_bytes,
} InitDataBlockKind;

// Static address to global variable or ROM area.
typedef struct {
    const char* label;
} InitDataBlockAddr;

// Static byte array.
typedef struct {
    size_t len;
    const char* buf;
} InitDataBlockBytes;

typedef struct {
    InitDataBlockKind kind;
    union {
        InitDataBlockAddr addr;
        InitDataBlockBytes bytes;
    } as;
} InitDataBlock;

typedef struct {
    size_t len;
    size_t capacity;
    InitDataBlock* blocks;
} InitData;

InitData* eval_init_expr(AstNode* expr, Type* ty);

#endif
