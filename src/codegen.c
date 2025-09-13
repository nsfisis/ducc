#include "codegen.h"
#include <string.h>
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
} CodeGen;

static CodeGen* codegen_new(FILE* out) {
    CodeGen* g = calloc(1, sizeof(CodeGen));
    g->out = out;
    g->next_label = 1;
    g->loop_labels = calloc(1024, sizeof(int));
    return g;
}

static int codegen_new_label(CodeGen* g) {
    int new_label = g->next_label;
    ++g->next_label;
    return new_label;
}

static void codegen_expr(CodeGen* g, AstNode* ast, GenMode gen_mode);
static void codegen_stmt(CodeGen* g, AstNode* ast);

static const char* param_reg(int n) {
    if (n == 0) {
        return "rdi";
    } else if (n == 1) {
        return "rsi";
    } else if (n == 2) {
        return "rdx";
    } else if (n == 3) {
        return "rcx";
    } else if (n == 4) {
        return "r8";
    } else if (n == 5) {
        return "r9";
    } else {
        unreachable();
    }
}

static void codegen_func_prologue(CodeGen* g, AstNode* ast) {
    fprintf(g->out, "  push rbp\n");
    fprintf(g->out, "  mov rbp, rsp\n");
    for (int i = 0; i < ast->node_params->node_len; ++i) {
        fprintf(g->out, "  push %s\n", param_reg(i));
    }
    // Note: rsp must be aligned to 8.
    fprintf(g->out, "  sub rsp, %d\n", to_aligned(ast->node_stack_size, 8));
}

static void codegen_func_epilogue(CodeGen* g, AstNode* ast) {
    fprintf(g->out, "  mov rsp, rbp\n");
    fprintf(g->out, "  pop rbp\n");
    fprintf(g->out, "  ret\n");
}

static void codegen_int_expr(CodeGen* g, AstNode* ast) {
    fprintf(g->out, "  push %d\n", ast->node_int_value);
}

static void codegen_str_expr(CodeGen* g, AstNode* ast) {
    fprintf(g->out, "  mov rax, OFFSET FLAG:.Lstr__%d\n", ast->node_idx);
    fprintf(g->out, "  push rax\n");
}

static void codegen_unary_expr(CodeGen* g, AstNode* ast) {
    codegen_expr(g, ast->node_operand, GenMode_rval);
    if (ast->node_op == TokenKind_not) {
        fprintf(g->out, "  pop rax\n");
        fprintf(g->out, "  mov rdi, 0\n");
        fprintf(g->out, "  cmp rax, rdi\n");
        fprintf(g->out, "  sete al\n");
        fprintf(g->out, "  movzb rax, al\n");
        fprintf(g->out, "  push rax\n");
    } else {
        unreachable();
    }
}

static void codegen_ref_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    codegen_expr(g, ast->node_operand, GenMode_lval);
}

// "reg" stores the address of the expression to be pushed.
static void codegen_push_expr(CodeGen* g, const char* reg, int size) {
    if (size == 1) {
        fprintf(g->out, "  movsx %s, BYTE PTR [%s]\n", reg, reg);
        fprintf(g->out, "  push %s\n", reg);
    } else if (size == 2) {
        fprintf(g->out, "  movsx %s, WORD PTR [%s]\n", reg, reg);
        fprintf(g->out, "  push %s\n", reg);
    } else if (size == 4) {
        fprintf(g->out, "  movsxd %s, DWORD PTR [%s]\n", reg, reg);
        fprintf(g->out, "  push %s\n", reg);
    } else if (size == 8) {
        fprintf(g->out, "  mov %s, [%s]\n", reg, reg);
        fprintf(g->out, "  push %s\n", reg);
    } else {
        // Perform bitwise copy. Use r10 register as temporary space.
        // Note: rsp must be aligned to 8.
        fprintf(g->out, "  sub rsp, %d\n", to_aligned(size, 8));
        for (int i = 0; i < size; ++i) {
            // Copy a sinle byte from the address that "reg" points to to the stack via r10 register.
            fprintf(g->out, "  mov r10b, [%s+%d]\n", reg, i);
            fprintf(g->out, "  mov [rsp+%d], r10b\n", i);
        }
    }
}

static void codegen_lval2rval(CodeGen* g, Type* ty) {
    if (ty->kind == TypeKind_array) {
        return;
    }

    fprintf(g->out, "  pop rax\n");
    codegen_push_expr(g, "rax", type_sizeof(ty));
}

static void codegen_deref_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    codegen_expr(g, ast->node_operand, GenMode_rval);
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(g, ast->node_operand->ty->base);
    }
}

static void codegen_cast_expr(CodeGen* g, AstNode* ast) {
    codegen_expr(g, ast->node_operand, GenMode_rval);

    int src_size = type_sizeof(ast->node_operand->ty);
    int dst_size = type_sizeof(ast->ty);

    if (src_size == dst_size)
        return;

    fprintf(g->out, "  pop rax\n");

    if (dst_size == 1) {
        fprintf(g->out, "  movsx rax, al\n");
    } else if (dst_size == 2) {
        if (src_size == 1) {
            fprintf(g->out, "  movsx rax, al\n");
        } else {
            fprintf(g->out, "  movsx rax, ax\n");
        }
    } else if (dst_size == 4) {
        if (src_size == 1) {
            fprintf(g->out, "  movsx rax, al\n");
        } else if (src_size == 2) {
            fprintf(g->out, "  movsx rax, ax\n");
        } else {
            fprintf(g->out, "  movsxd rax, eax\n");
        }
    } else if (dst_size == 8) {
        if (src_size == 1) {
            fprintf(g->out, "  movsx rax, al\n");
        } else if (src_size == 2) {
            fprintf(g->out, "  movsx rax, ax\n");
        } else if (src_size == 4) {
            fprintf(g->out, "  movsxd rax, eax\n");
        }
    }

    fprintf(g->out, "  push rax\n");
}

static void codegen_logical_expr(CodeGen* g, AstNode* ast) {
    int label = codegen_new_label(g);

    if (ast->node_op == TokenKind_andand) {
        codegen_expr(g, ast->node_lhs, GenMode_rval);
        fprintf(g->out, "  pop rax\n");
        fprintf(g->out, "  cmp rax, 0\n");
        fprintf(g->out, "  je .Lelse%d\n", label);
        codegen_expr(g, ast->node_rhs, GenMode_rval);
        fprintf(g->out, "  jmp .Lend%d\n", label);
        fprintf(g->out, ".Lelse%d:\n", label);
        fprintf(g->out, "  push 0\n");
        fprintf(g->out, ".Lend%d:\n", label);
    } else {
        codegen_expr(g, ast->node_lhs, GenMode_rval);
        fprintf(g->out, "  pop rax\n");
        fprintf(g->out, "  cmp rax, 0\n");
        fprintf(g->out, "  je .Lelse%d\n", label);
        fprintf(g->out, "  push 1\n");
        fprintf(g->out, "  jmp .Lend%d\n", label);
        fprintf(g->out, ".Lelse%d:\n", label);
        codegen_expr(g, ast->node_rhs, GenMode_rval);
        fprintf(g->out, ".Lend%d:\n", label);
    }
}

static void codegen_binary_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    codegen_expr(g, ast->node_lhs, gen_mode);
    codegen_expr(g, ast->node_rhs, gen_mode);
    fprintf(g->out, "  pop rdi\n");
    fprintf(g->out, "  pop rax\n");
    if (ast->node_op == TokenKind_plus) {
        fprintf(g->out, "  add rax, rdi\n");
    } else if (ast->node_op == TokenKind_minus) {
        fprintf(g->out, "  sub rax, rdi\n");
    } else if (ast->node_op == TokenKind_star) {
        fprintf(g->out, "  imul rax, rdi\n");
    } else if (ast->node_op == TokenKind_slash) {
        fprintf(g->out, "  cqo\n");
        fprintf(g->out, "  idiv rdi\n");
    } else if (ast->node_op == TokenKind_percent) {
        fprintf(g->out, "  cqo\n");
        fprintf(g->out, "  idiv rdi\n");
        fprintf(g->out, "  mov rax, rdx\n");
    } else if (ast->node_op == TokenKind_and) {
        fprintf(g->out, "  and rax, rdi\n");
    } else if (ast->node_op == TokenKind_or) {
        fprintf(g->out, "  or rax, rdi\n");
    } else if (ast->node_op == TokenKind_xor) {
        fprintf(g->out, "  xor rax, rdi\n");
    } else if (ast->node_op == TokenKind_lshift) {
        fprintf(g->out, "  mov rcx, rdi\n");
        fprintf(g->out, "  shl rax, cl\n");
    } else if (ast->node_op == TokenKind_rshift) {
        // TODO: check if the operand is signed or unsigned
        fprintf(g->out, "  mov rcx, rdi\n");
        fprintf(g->out, "  sar rax, cl\n");
    } else if (ast->node_op == TokenKind_eq) {
        fprintf(g->out, "  cmp rax, rdi\n");
        fprintf(g->out, "  sete al\n");
        fprintf(g->out, "  movzb rax, al\n");
    } else if (ast->node_op == TokenKind_ne) {
        fprintf(g->out, "  cmp rax, rdi\n");
        fprintf(g->out, "  setne al\n");
        fprintf(g->out, "  movzb rax, al\n");
    } else if (ast->node_op == TokenKind_lt) {
        fprintf(g->out, "  cmp rax, rdi\n");
        fprintf(g->out, "  setl al\n");
        fprintf(g->out, "  movzb rax, al\n");
    } else if (ast->node_op == TokenKind_le) {
        fprintf(g->out, "  cmp rax, rdi\n");
        fprintf(g->out, "  setle al\n");
        fprintf(g->out, "  movzb rax, al\n");
    } else {
        unreachable();
    }
    fprintf(g->out, "  push rax\n");
}

static void codegen_cond_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    int label = codegen_new_label(g);

    codegen_expr(g, ast->node_cond, GenMode_rval);
    fprintf(g->out, "  pop rax\n");
    fprintf(g->out, "  cmp rax, 0\n");
    fprintf(g->out, "  je .Lelse%d\n", label);
    codegen_expr(g, ast->node_then, gen_mode);
    fprintf(g->out, "  jmp .Lend%d\n", label);
    fprintf(g->out, ".Lelse%d:\n", label);
    codegen_expr(g, ast->node_else, gen_mode);
    fprintf(g->out, ".Lend%d:\n", label);
}

static void codegen_assign_expr_helper(CodeGen* g, AstNode* ast) {
    if (ast->node_op == TokenKind_assign) {
        return;
    }

    fprintf(g->out, "  pop rdi\n");
    fprintf(g->out, "  push [rsp]\n");
    codegen_lval2rval(g, ast->node_lhs->ty);
    fprintf(g->out, "  pop rax\n");

    if (ast->node_op == TokenKind_assign_add) {
        fprintf(g->out, "  add rax, rdi\n");
    } else if (ast->node_op == TokenKind_assign_sub) {
        fprintf(g->out, "  sub rax, rdi\n");
    } else if (ast->node_op == TokenKind_assign_mul) {
        fprintf(g->out, "  imul rax, rdi\n");
    } else if (ast->node_op == TokenKind_assign_div) {
        fprintf(g->out, "  cqo\n");
        fprintf(g->out, "  idiv rdi\n");
    } else if (ast->node_op == TokenKind_assign_mod) {
        fprintf(g->out, "  cqo\n");
        fprintf(g->out, "  idiv rdi\n");
        fprintf(g->out, "  mov rax, rdx\n");
    } else {
        unreachable();
    }

    fprintf(g->out, "  push rax\n");
}

static void codegen_assign_expr(CodeGen* g, AstNode* ast) {
    int sizeof_lhs = type_sizeof(ast->node_lhs->ty);

    codegen_expr(g, ast->node_lhs, GenMode_lval);
    codegen_expr(g, ast->node_rhs, GenMode_rval);
    codegen_assign_expr_helper(g, ast);
    if (sizeof_lhs == 1) {
        fprintf(g->out, "  pop rdi\n");
        fprintf(g->out, "  pop rax\n");
        fprintf(g->out, "  mov BYTE PTR [rax], dil\n");
        fprintf(g->out, "  push rdi\n");
    } else if (sizeof_lhs == 2) {
        fprintf(g->out, "  pop rdi\n");
        fprintf(g->out, "  pop rax\n");
        fprintf(g->out, "  mov WORD PTR [rax], di\n");
        fprintf(g->out, "  push rdi\n");
    } else if (sizeof_lhs == 4) {
        fprintf(g->out, "  pop rdi\n");
        fprintf(g->out, "  pop rax\n");
        fprintf(g->out, "  mov DWORD PTR [rax], edi\n");
        fprintf(g->out, "  push rdi\n");
    } else if (sizeof_lhs == 8) {
        fprintf(g->out, "  pop rdi\n");
        fprintf(g->out, "  pop rax\n");
        fprintf(g->out, "  mov [rax], rdi\n");
        fprintf(g->out, "  push rdi\n");
    } else {
        if (ast->node_op != TokenKind_assign) {
            unimplemented();
        }
        // Note: rsp must be aligned to 8.
        int aligned_size = to_aligned(sizeof_lhs, 8);
        fprintf(g->out, "  mov rax, [rsp+%d]\n", aligned_size);
        // Perform bitwise copy. Use r10 register as temporary space.
        for (int i = 0; i < aligned_size; ++i) {
            // Copy a sinle byte from the stack to the address that rax points to via r10 register.
            fprintf(g->out, "  mov r10b, [rsp+%d]\n", i);
            fprintf(g->out, "  mov [rax+%d], r10b\n", i);
        }
        // Pop the RHS value and the LHS address.
        fprintf(g->out, "  add rsp, %d\n", aligned_size + 8);
        // TODO: dummy
        fprintf(g->out, "  push 0\n");
    }
}

static void codegen_func_call(CodeGen* g, AstNode* ast) {
    const char* func_name = ast->name;

    if (strcmp(func_name, "__ducc_va_start") == 0) {
        fprintf(g->out, "  # __ducc_va_start BEGIN\n");
        AstNode* va_list_args = &ast->node_args->node_items[0];
        codegen_expr(g, va_list_args, GenMode_rval);
        fprintf(g->out, "  pop rdi\n");

        // Allocate save area.
        fprintf(g->out, "  sub rsp, 48\n");

        fprintf(g->out, "  mov [rsp+ 0], rdi\n");
        fprintf(g->out, "  mov [rsp+ 8], rsi\n");
        fprintf(g->out, "  mov [rsp+16], rdx\n");
        fprintf(g->out, "  mov [rsp+24], rcx\n");
        fprintf(g->out, "  mov [rsp+32], r8\n");
        fprintf(g->out, "  mov [rsp+40], r9\n");

        // Initialize va_list.
        fprintf(g->out, "  mov DWORD PTR [rdi], 8\n"); // gp_offset
        fprintf(g->out, "  mov DWORD PTR [rdi+4], 0\n"); // fp_offset
        fprintf(g->out, "  lea rax, [rbp+16]\n"); // overflow_arg_area
        fprintf(g->out, "  mov QWORD PTR [rdi+8], rax\n");
        fprintf(g->out, "  mov QWORD PTR [rdi+16], rsp\n"); // reg_save_area

        fprintf(g->out, "  push 0\n"); // dummy return value
        fprintf(g->out, "  # __ducc_va_start END\n");
        return;
    }

    AstNode* args = ast->node_args;
    for (int i = 0; i < args->node_len; ++i) {
        AstNode* arg = args->node_items + i;
        codegen_expr(g, arg, GenMode_rval);
    }
    for (int i = args->node_len - 1; i >= 0; --i) {
        fprintf(g->out, "  pop %s\n", param_reg(i));
    }

    int label = codegen_new_label(g);

    fprintf(g->out, "  mov rax, rsp\n");
    fprintf(g->out, "  and rax, 15\n");
    fprintf(g->out, "  cmp rax, 0\n");
    fprintf(g->out, "  je .Laligned%d\n", label);

    fprintf(g->out, "  mov rax, 0\n");
    fprintf(g->out, "  sub rsp, 8\n");
    fprintf(g->out, "  call %s\n", func_name);
    fprintf(g->out, "  add rsp, 8\n");
    fprintf(g->out, "  push rax\n");

    fprintf(g->out, "  jmp .Lend%d\n", label);
    fprintf(g->out, ".Laligned%d:\n", label);

    fprintf(g->out, "  mov rax, 0\n");
    fprintf(g->out, "  call %s\n", func_name);
    fprintf(g->out, "  push rax\n");

    fprintf(g->out, ".Lend%d:\n", label);
}

static void codegen_lvar(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    fprintf(g->out, "  mov rax, rbp\n");
    fprintf(g->out, "  sub rax, %d\n", ast->node_stack_offset);
    fprintf(g->out, "  push rax\n");
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(g, ast->ty);
    }
}

static void codegen_gvar(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    fprintf(g->out, "  lea rax, %s[rip]\n", ast->name);
    fprintf(g->out, "  push rax\n");
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(g, ast->ty);
    }
}

static void codegen_composite_expr(CodeGen* g, AstNode* ast) {
    // Standard C does not have composite expression, but ducc internally has.
    for (int i = 0; i < ast->node_len; ++i) {
        AstNode* expr = ast->node_items + i;
        codegen_expr(g, expr, GenMode_rval);
        if (i != ast->node_len - 1) {
            // TODO: the expression on the stack can be more than 8 bytes.
            fprintf(g->out, "  pop rax\n");
        }
    }
}

static void codegen_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    if (ast->kind == AstNodeKind_int_expr) {
        codegen_int_expr(g, ast);
    } else if (ast->kind == AstNodeKind_str_expr) {
        codegen_str_expr(g, ast);
    } else if (ast->kind == AstNodeKind_unary_expr) {
        codegen_unary_expr(g, ast);
    } else if (ast->kind == AstNodeKind_ref_expr) {
        codegen_ref_expr(g, ast, gen_mode);
    } else if (ast->kind == AstNodeKind_deref_expr) {
        codegen_deref_expr(g, ast, gen_mode);
    } else if (ast->kind == AstNodeKind_cast_expr) {
        codegen_cast_expr(g, ast);
    } else if (ast->kind == AstNodeKind_binary_expr) {
        codegen_binary_expr(g, ast, gen_mode);
    } else if (ast->kind == AstNodeKind_cond_expr) {
        codegen_cond_expr(g, ast, gen_mode);
    } else if (ast->kind == AstNodeKind_logical_expr) {
        codegen_logical_expr(g, ast);
    } else if (ast->kind == AstNodeKind_assign_expr) {
        codegen_assign_expr(g, ast);
    } else if (ast->kind == AstNodeKind_func_call) {
        codegen_func_call(g, ast);
    } else if (ast->kind == AstNodeKind_lvar) {
        codegen_lvar(g, ast, gen_mode);
    } else if (ast->kind == AstNodeKind_gvar) {
        codegen_gvar(g, ast, gen_mode);
    } else if (ast->kind == AstNodeKind_list) {
        codegen_composite_expr(g, ast);
    } else {
        unreachable();
    }
}

static void codegen_return_stmt(CodeGen* g, AstNode* ast) {
    if (ast->node_expr) {
        codegen_expr(g, ast->node_expr, GenMode_rval);
        fprintf(g->out, "  pop rax\n");
    }
    codegen_func_epilogue(g, ast);
}

static void codegen_if_stmt(CodeGen* g, AstNode* ast) {
    int label = codegen_new_label(g);

    codegen_expr(g, ast->node_cond, GenMode_rval);
    fprintf(g->out, "  pop rax\n");
    fprintf(g->out, "  cmp rax, 0\n");
    fprintf(g->out, "  je .Lelse%d\n", label);
    codegen_stmt(g, ast->node_then);
    fprintf(g->out, "  jmp .Lend%d\n", label);
    fprintf(g->out, ".Lelse%d:\n", label);
    if (ast->node_else) {
        codegen_stmt(g, ast->node_else);
    }
    fprintf(g->out, ".Lend%d:\n", label);
}

static void codegen_for_stmt(CodeGen* g, AstNode* ast) {
    int label = codegen_new_label(g);
    ++g->loop_labels;
    *g->loop_labels = label;

    if (ast->node_init) {
        codegen_expr(g, ast->node_init, GenMode_rval);
        fprintf(g->out, "  pop rax\n");
    }
    fprintf(g->out, ".Lbegin%d:\n", label);
    codegen_expr(g, ast->node_cond, GenMode_rval);
    fprintf(g->out, "  pop rax\n");
    fprintf(g->out, "  cmp rax, 0\n");
    fprintf(g->out, "  je .Lend%d\n", label);
    codegen_stmt(g, ast->node_body);
    fprintf(g->out, ".Lcontinue%d:\n", label);
    if (ast->node_update) {
        codegen_expr(g, ast->node_update, GenMode_rval);
        fprintf(g->out, "  pop rax\n");
    }
    fprintf(g->out, "  jmp .Lbegin%d\n", label);
    fprintf(g->out, ".Lend%d:\n", label);

    --g->loop_labels;
}

static void codegen_do_while_stmt(CodeGen* g, AstNode* ast) {
    int label = codegen_new_label(g);
    ++g->loop_labels;
    *g->loop_labels = label;

    fprintf(g->out, ".Lbegin%d:\n", label);
    codegen_stmt(g, ast->node_body);
    fprintf(g->out, ".Lcontinue%d:\n", label);
    codegen_expr(g, ast->node_cond, GenMode_rval);
    fprintf(g->out, "  pop rax\n");
    fprintf(g->out, "  cmp rax, 0\n");
    fprintf(g->out, "  je .Lend%d\n", label);
    fprintf(g->out, "  jmp .Lbegin%d\n", label);
    fprintf(g->out, ".Lend%d:\n", label);

    --g->loop_labels;
}

static void codegen_break_stmt(CodeGen* g, AstNode* ast) {
    int label = *g->loop_labels;
    fprintf(g->out, "  jmp .Lend%d\n", label);
}

static void codegen_continue_stmt(CodeGen* g, AstNode* ast) {
    int label = *g->loop_labels;
    fprintf(g->out, "  jmp .Lcontinue%d\n", label);
}

static void codegen_expr_stmt(CodeGen* g, AstNode* ast) {
    codegen_expr(g, ast->node_expr, GenMode_rval);
    // TODO: the expression on the stack can be more than 8 bytes.
    fprintf(g->out, "  pop rax\n");
}

static void codegen_var_decl(CodeGen* g, AstNode* ast) {
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
    } else if (ast->kind == AstNodeKind_if_stmt) {
        codegen_if_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_for_stmt) {
        codegen_for_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_do_while_stmt) {
        codegen_do_while_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_break_stmt) {
        codegen_break_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_continue_stmt) {
        codegen_continue_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_expr_stmt) {
        codegen_expr_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_lvar_decl) {
        codegen_var_decl(g, ast);
    } else if (ast->kind == AstNodeKind_nop) {
        codegen_nop(g, ast);
    } else {
        unreachable();
    }
}

static void codegen_func(CodeGen* g, AstNode* ast) {
    g->current_func = ast;

    if (ast->ty->result->storage_class != StorageClass_static) {
        fprintf(g->out, ".globl %s\n", ast->name);
    }
    fprintf(g->out, "%s:\n", ast->name);

    codegen_func_prologue(g, ast);
    codegen_stmt(g, ast->node_body);
    if (strcmp(ast->name, "main") == 0) {
        // C99: 5.1.2.2.3
        fprintf(g->out, "  mov rax, 0\n");
    }
    codegen_func_epilogue(g, ast);

    fprintf(g->out, "\n");
    g->current_func = NULL;
}

void codegen(Program* prog, FILE* out) {
    CodeGen* g = codegen_new(out);

    fprintf(g->out, ".intel_syntax noprefix\n\n");

    // For GNU ld:
    // https://sourceware.org/binutils/docs/ld/Options.html
    fprintf(g->out, ".section .note.GNU-stack,\"\",@progbits\n\n");

    fprintf(g->out, ".section .rodata\n\n");
    for (int i = 0; prog->str_literals[i]; ++i) {
        fprintf(g->out, ".Lstr__%d:\n", i + 1);
        fprintf(g->out, "  .string \"%s\"\n\n", prog->str_literals[i]);
    }

    fprintf(g->out, ".data\n\n");
    for (int i = 0; i < prog->vars->node_len; ++i) {
        AstNode* var = prog->vars->node_items + i;
        if (var->node_expr) {
            if (type_sizeof(var->ty) == 1)
                fprintf(g->out, "  %s: .byte %d\n", var->name, var->node_expr->node_int_value);
            else if (type_sizeof(var->ty) == 2)
                fprintf(g->out, "  %s: .word %d\n", var->name, var->node_expr->node_int_value);
            else if (type_sizeof(var->ty) == 4)
                fprintf(g->out, "  %s: .int %d\n", var->name, var->node_expr->node_int_value);
            else
                unimplemented();
        } else {
            fprintf(g->out, "  %s: .zero %d\n", var->name, type_sizeof(var->ty));
        }
    }

    fprintf(g->out, ".text\n\n");
    for (int i = 0; i < prog->funcs->node_len; ++i) {
        AstNode* func = prog->funcs->node_items + i;
        codegen_func(g, func);
    }
}
