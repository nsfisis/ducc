#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "common.h"
#include "preprocess.h"

typedef enum {
    GenMode_lval,
    GenMode_rval,
} GenMode;

typedef struct {
    FILE* out;
    int next_label;
    int* loop_labels;
    AstNode* current_func;
    int switch_label;
} CodeGen;

static CodeGen* codegen_new(FILE* out) {
    CodeGen* g = calloc(1, sizeof(CodeGen));
    g->out = out;
    g->next_label = 1;
    g->loop_labels = calloc(1024, sizeof(int));
    g->switch_label = -1;
    return g;
}

static void codegen_expr(CodeGen* g, AstNode* ast, GenMode gen_mode);
static void codegen_stmt(CodeGen* g, AstNode* ast);

static void codegen_func_prologue(CodeGen* g, AstNode* ast) {
    for (int i = 0; i < ast->node_params->node_len; ++i) {
        fprintf(g->out, " (param $l_%s i32)", ast->node_params->node_items[i].name);
    }
    fprintf(g->out, " (result i32)\n");
}

static void codegen_func_epilogue(CodeGen* g, AstNode* ast) {
}

static void codegen_binary_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    codegen_expr(g, ast->node_lhs, gen_mode);
    codegen_expr(g, ast->node_rhs, gen_mode);
    if (ast->node_op == TokenKind_plus) {
        fprintf(g->out, "i32.add\n");
    } else {
        unreachable();
    }
}

static void codegen_lvar(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    fprintf(g->out, "  local.get $l_%s\n", ast->name);
}

static void codegen_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    if (ast->kind == AstNodeKind_binary_expr) {
        codegen_binary_expr(g, ast, gen_mode);
    } else if (ast->kind == AstNodeKind_lvar) {
        codegen_lvar(g, ast, gen_mode);
    } else {
        unreachable();
    }
}

static void codegen_return_stmt(CodeGen* g, AstNode* ast) {
    if (ast->node_expr) {
        codegen_expr(g, ast->node_expr, GenMode_rval);
    }
    codegen_func_epilogue(g, ast);
}

static void codegen_nop(CodeGen* g, AstNode* ast) {
}

static void codegen_block_stmt(CodeGen* g, AstNode* ast) {
    for (int i = 0; i < ast->node_len; ++i) {
        AstNode* stmt = ast->node_items + i;
        codegen_stmt(g, stmt);
    }
}

static void codegen_stmt(CodeGen* g, AstNode* ast) {
    if (ast->kind == AstNodeKind_list) {
        codegen_block_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_return_stmt) {
        codegen_return_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_nop) {
        codegen_nop(g, ast);
    } else {
        unreachable();
    }
}

static void codegen_func(CodeGen* g, AstNode* ast) {
    g->current_func = ast;

    fprintf(g->out, "(func (export \"%s\") \n", ast->name);

    codegen_func_prologue(g, ast);
    codegen_stmt(g, ast->node_body);
    codegen_func_epilogue(g, ast);

    fprintf(g->out, ")\n");
    g->current_func = NULL;
}

void codegen_wasm(Program* prog, FILE* out) {
    CodeGen* g = codegen_new(out);

    fprintf(g->out, "(module\n");

    for (int i = 0; i < prog->funcs->node_len; ++i) {
        AstNode* func = prog->funcs->node_items + i;
        codegen_func(g, func);
    }

    fprintf(g->out, ")\n");
}
