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

static void codegen_func_prologue(CodeGen* g, FuncDefNode* func_def) {
    for (int i = 0; i < func_def->params->as.list->len; ++i) {
        fprintf(g->out, " (param $l_%s i32)", func_def->params->as.list->items[i].as.param->name);
    }
    fprintf(g->out, " (result i32)\n");
}

static void codegen_func_epilogue(CodeGen*) {
}

static void codegen_int_expr(CodeGen* g, IntExprNode* expr) {
    fprintf(g->out, "  i32.const %d\n", expr->value);
}

static void codegen_binary_expr(CodeGen* g, BinaryExprNode* expr, GenMode gen_mode) {
    codegen_expr(g, expr->lhs, gen_mode);
    codegen_expr(g, expr->rhs, gen_mode);
    if (expr->op == TokenKind_plus) {
        fprintf(g->out, "  i32.add\n");
    } else if (expr->op == TokenKind_minus) {
        fprintf(g->out, "  i32.sub\n");
    } else if (expr->op == TokenKind_le) {
        fprintf(g->out, "  i32.le_s\n");
    } else {
        unreachable();
    }
}

static void codegen_lvar(CodeGen* g, LvarNode* lvar, GenMode) {
    fprintf(g->out, "  local.get $l_%s\n", lvar->name);
}

static void codegen_func_call(CodeGen* g, FuncCallNode* call) {
    AstNode* args = call->args;
    for (int i = 0; i < args->as.list->len; ++i) {
        AstNode* arg = args->as.list->items + i;
        codegen_expr(g, arg, GenMode_rval);
    }
    fprintf(g->out, "  call $%s\n", call->name);
}

static void codegen_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    if (ast->kind == AstNodeKind_int_expr) {
        codegen_int_expr(g, ast->as.int_expr);
    } else if (ast->kind == AstNodeKind_binary_expr) {
        codegen_binary_expr(g, ast->as.binary_expr, gen_mode);
    } else if (ast->kind == AstNodeKind_lvar) {
        codegen_lvar(g, ast->as.lvar, gen_mode);
    } else if (ast->kind == AstNodeKind_func_call) {
        codegen_func_call(g, ast->as.func_call);
    } else {
        unreachable();
    }
}

static void codegen_return_stmt(CodeGen* g, ReturnStmtNode* stmt) {
    if (stmt->expr) {
        codegen_expr(g, stmt->expr, GenMode_rval);
    }
    fprintf(g->out, "  return\n");
}

static void codegen_if_stmt(CodeGen* g, IfStmtNode* stmt) {
    codegen_expr(g, stmt->cond, GenMode_rval);
    fprintf(g->out, "  (if (result i32)\n");
    fprintf(g->out, "    (then\n");
    codegen_stmt(g, stmt->then);
    fprintf(g->out, "    )\n");
    if (stmt->else_) {
        fprintf(g->out, "    (else\n");
        codegen_stmt(g, stmt->else_);
        fprintf(g->out, "    )\n");
    } else {
        fprintf(g->out, "    (else\n");
        fprintf(g->out, "      i32.const 0\n");
        fprintf(g->out, "    )\n");
    }
    fprintf(g->out, "  )\n");
}

static void codegen_block_stmt(CodeGen* g, AstNode* ast) {
    for (int i = 0; i < ast->as.list->len; ++i) {
        AstNode* stmt = ast->as.list->items + i;
        codegen_stmt(g, stmt);
    }
}

static void codegen_stmt(CodeGen* g, AstNode* ast) {
    if (ast->kind == AstNodeKind_list) {
        codegen_block_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_return_stmt) {
        codegen_return_stmt(g, ast->as.return_stmt);
    } else if (ast->kind == AstNodeKind_if_stmt) {
        codegen_if_stmt(g, ast->as.if_stmt);
    } else if (ast->kind == AstNodeKind_nop) {
        // Do nothing.
    } else {
        unreachable();
    }
}

static void codegen_func(CodeGen* g, AstNode* ast) {
    g->current_func = ast;

    fprintf(g->out, "(func $%s (export \"%s\")", ast->as.func_def->name, ast->as.func_def->name);

    codegen_func_prologue(g, ast->as.func_def);
    codegen_stmt(g, ast->as.func_def->body);
    codegen_func_epilogue(g);

    fprintf(g->out, ")\n");
    g->current_func = NULL;
}

void codegen_wasm(Program* prog, FILE* out) {
    CodeGen* g = codegen_new(out);

    fprintf(g->out, "(module\n");

    for (int i = 0; i < prog->funcs->as.list->len; ++i) {
        AstNode* func = prog->funcs->as.list->items + i;
        codegen_func(g, func);
    }

    fprintf(g->out, ")\n");
}
