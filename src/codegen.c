#include "codegen.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "parse.h"
#include "preprocess.h"

typedef enum {
    GenMode_lval,
    GenMode_rval,
} GenMode;

typedef struct {
    Program* prog;
    FILE* out;
    int next_label;
    int* loop_labels;
    AstNode* current_func;
    int switch_label;
} CodeGen;

static CodeGen* codegen_new(Program* prog, FILE* out) {
    CodeGen* g = calloc(1, sizeof(CodeGen));
    g->prog = prog;
    g->out = out;
    g->next_label = 1;
    g->loop_labels = calloc(1024, sizeof(int));
    g->switch_label = -1;
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

static void codegen_func_prologue(CodeGen* g, FuncDefNode* func_def) {
    fprintf(g->out, "  push rbp\n");
    fprintf(g->out, "  mov rbp, rsp\n");
    for (int i = 0, j = 0; i < func_def->params->as.list->len; ++i) {
        AstNode* param = &func_def->params->as.list->items[i];
        if (param->as.param->stack_offset >= 0) {
            fprintf(g->out, "  push %s\n", param_reg(j++));
        }
    }
    // Note: rsp must be aligned to 8.
    fprintf(g->out, "  sub rsp, %d\n", to_aligned(func_def->stack_size, 8));
}

static void codegen_func_epilogue(CodeGen* g) {
    fprintf(g->out, "  mov rsp, rbp\n");
    fprintf(g->out, "  pop rbp\n");
    fprintf(g->out, "  ret\n");
}

static void codegen_int_expr(CodeGen* g, IntExprNode* expr) {
    fprintf(g->out, "  mov rax, %d\n", expr->value);
}

static void codegen_str_expr(CodeGen* g, StrExprNode* expr) {
    fprintf(g->out, "  lea rax, .Lstr__%d[rip]\n", expr->idx);
}

static void codegen_unary_expr(CodeGen* g, UnaryExprNode* expr) {
    codegen_expr(g, expr->operand, GenMode_rval);
    if (expr->op == TokenKind_not) {
        fprintf(g->out, "  mov rdi, 0\n");
        fprintf(g->out, "  cmp rax, rdi\n");
        fprintf(g->out, "  sete al\n");
        fprintf(g->out, "  movzb rax, al\n");
    } else if (expr->op == TokenKind_tilde) {
        fprintf(g->out, "  not rax\n");
    } else {
        unreachable();
    }
}

static void codegen_ref_expr(CodeGen* g, RefExprNode* expr) {
    codegen_expr(g, expr->operand, GenMode_lval);
}

static void codegen_lval2rval(CodeGen* g, Type* ty) {
    if (ty->kind == TypeKind_array) {
        return;
    }

    int size = type_sizeof(ty);
    if (size == 1) {
        fprintf(g->out, "  movsx rax, BYTE PTR [rax]\n");
    } else if (size == 2) {
        fprintf(g->out, "  movsx rax, WORD PTR [rax]\n");
    } else if (size == 4) {
        fprintf(g->out, "  movsxd rax, DWORD PTR [rax]\n");
    } else if (size == 8) {
        fprintf(g->out, "  mov rax, [rax]\n");
    } else {
        // Do nothing.
    }
}

static void codegen_deref_expr(CodeGen* g, DerefExprNode* expr, GenMode gen_mode) {
    codegen_expr(g, expr->operand, GenMode_rval);
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(g, expr->operand->ty->base);
    }
}

static void codegen_cast_expr(CodeGen* g, CastExprNode* expr, Type* ty) {
    codegen_expr(g, expr->operand, GenMode_rval);

    // (void) cast does nothing.
    if (ty->kind == TypeKind_void)
        return;

    int src_size = type_sizeof(expr->operand->ty);
    int dst_size = type_sizeof(ty);

    if (src_size == dst_size)
        return;

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
}

static void codegen_logical_expr(CodeGen* g, LogicalExprNode* expr) {
    int label = codegen_new_label(g);

    if (expr->op == TokenKind_andand) {
        codegen_expr(g, expr->lhs, GenMode_rval);
        fprintf(g->out, "  cmp rax, 0\n");
        fprintf(g->out, "  je .Lelse%d\n", label);
        codegen_expr(g, expr->rhs, GenMode_rval);
        fprintf(g->out, "  jmp .Lend%d\n", label);
        fprintf(g->out, ".Lelse%d:\n", label);
        fprintf(g->out, "  mov rax, 0\n");
        fprintf(g->out, ".Lend%d:\n", label);
    } else {
        codegen_expr(g, expr->lhs, GenMode_rval);
        fprintf(g->out, "  cmp rax, 0\n");
        fprintf(g->out, "  je .Lelse%d\n", label);
        fprintf(g->out, "  mov rax, 1\n");
        fprintf(g->out, "  jmp .Lend%d\n", label);
        fprintf(g->out, ".Lelse%d:\n", label);
        codegen_expr(g, expr->rhs, GenMode_rval);
        fprintf(g->out, ".Lend%d:\n", label);
    }
}

static void codegen_binary_expr(CodeGen* g, BinaryExprNode* expr, GenMode gen_mode) {
    codegen_expr(g, expr->lhs, gen_mode);
    fprintf(g->out, "  push rax\n");
    codegen_expr(g, expr->rhs, gen_mode);
    fprintf(g->out, "  mov rdi, rax\n");
    fprintf(g->out, "  pop rax\n");
    // rax=lhs, rdi=rhs
    if (expr->op == TokenKind_plus) {
        fprintf(g->out, "  add rax, rdi\n");
    } else if (expr->op == TokenKind_minus) {
        fprintf(g->out, "  sub rax, rdi\n");
    } else if (expr->op == TokenKind_star) {
        fprintf(g->out, "  imul rax, rdi\n");
    } else if (expr->op == TokenKind_slash) {
        fprintf(g->out, "  cqo\n");
        fprintf(g->out, "  idiv rdi\n");
    } else if (expr->op == TokenKind_percent) {
        fprintf(g->out, "  cqo\n");
        fprintf(g->out, "  idiv rdi\n");
        fprintf(g->out, "  mov rax, rdx\n");
    } else if (expr->op == TokenKind_and) {
        fprintf(g->out, "  and rax, rdi\n");
    } else if (expr->op == TokenKind_or) {
        fprintf(g->out, "  or rax, rdi\n");
    } else if (expr->op == TokenKind_xor) {
        fprintf(g->out, "  xor rax, rdi\n");
    } else if (expr->op == TokenKind_lshift) {
        fprintf(g->out, "  mov rcx, rdi\n");
        fprintf(g->out, "  shl rax, cl\n");
    } else if (expr->op == TokenKind_rshift) {
        // TODO: check if the operand is signed or unsigned
        fprintf(g->out, "  mov rcx, rdi\n");
        fprintf(g->out, "  sar rax, cl\n");
    } else if (expr->op == TokenKind_eq) {
        fprintf(g->out, "  cmp rax, rdi\n");
        fprintf(g->out, "  sete al\n");
        fprintf(g->out, "  movzb rax, al\n");
    } else if (expr->op == TokenKind_ne) {
        fprintf(g->out, "  cmp rax, rdi\n");
        fprintf(g->out, "  setne al\n");
        fprintf(g->out, "  movzb rax, al\n");
    } else if (expr->op == TokenKind_lt) {
        fprintf(g->out, "  cmp rax, rdi\n");
        fprintf(g->out, "  setl al\n");
        fprintf(g->out, "  movzb rax, al\n");
    } else if (expr->op == TokenKind_le) {
        fprintf(g->out, "  cmp rax, rdi\n");
        fprintf(g->out, "  setle al\n");
        fprintf(g->out, "  movzb rax, al\n");
    } else {
        unreachable();
    }
}

static void codegen_cond_expr(CodeGen* g, CondExprNode* expr, GenMode gen_mode) {
    int label = codegen_new_label(g);

    codegen_expr(g, expr->cond, GenMode_rval);
    fprintf(g->out, "  cmp rax, 0\n");
    fprintf(g->out, "  je .Lelse%d\n", label);
    codegen_expr(g, expr->then, gen_mode);
    fprintf(g->out, "  jmp .Lend%d\n", label);
    fprintf(g->out, ".Lelse%d:\n", label);
    codegen_expr(g, expr->else_, gen_mode);
    fprintf(g->out, ".Lend%d:\n", label);
}

static void codegen_assign_expr_helper(CodeGen* g, AssignExprNode* expr) {
    if (expr->op == TokenKind_assign) {
        return;
    }

    fprintf(g->out, "  mov rdi, rax\n");
    fprintf(g->out, "  mov rax, [rsp]\n");
    codegen_lval2rval(g, expr->lhs->ty);

    if (expr->op == TokenKind_assign_add) {
        fprintf(g->out, "  add rax, rdi\n");
    } else if (expr->op == TokenKind_assign_sub) {
        fprintf(g->out, "  sub rax, rdi\n");
    } else if (expr->op == TokenKind_assign_mul) {
        fprintf(g->out, "  imul rax, rdi\n");
    } else if (expr->op == TokenKind_assign_div) {
        fprintf(g->out, "  cqo\n");
        fprintf(g->out, "  idiv rdi\n");
    } else if (expr->op == TokenKind_assign_mod) {
        fprintf(g->out, "  cqo\n");
        fprintf(g->out, "  idiv rdi\n");
        fprintf(g->out, "  mov rax, rdx\n");
    } else if (expr->op == TokenKind_assign_or) {
        fprintf(g->out, "  or rax, rdi\n");
    } else if (expr->op == TokenKind_assign_and) {
        fprintf(g->out, "  and rax, rdi\n");
    } else if (expr->op == TokenKind_assign_xor) {
        fprintf(g->out, "  xor rax, rdi\n");
    } else if (expr->op == TokenKind_assign_lshift) {
        fprintf(g->out, "  mov rcx, rdi\n");
        fprintf(g->out, "  shl rax, cl\n");
    } else if (expr->op == TokenKind_assign_rshift) {
        fprintf(g->out, "  mov rcx, rdi\n");
        fprintf(g->out, "  sar rax, cl\n");
    } else {
        unreachable();
    }
}

static void codegen_assign_expr(CodeGen* g, AssignExprNode* expr) {
    int sizeof_lhs = type_sizeof(expr->lhs->ty);

    codegen_expr(g, expr->lhs, GenMode_lval);
    fprintf(g->out, "  push rax\n");
    codegen_expr(g, expr->rhs, GenMode_rval);
    codegen_assign_expr_helper(g, expr);
    fprintf(g->out, "  pop rdi\n");
    if (sizeof_lhs == 1) {
        fprintf(g->out, "  mov BYTE PTR [rdi], al\n");
    } else if (sizeof_lhs == 2) {
        fprintf(g->out, "  mov WORD PTR [rdi], ax\n");
    } else if (sizeof_lhs == 4) {
        fprintf(g->out, "  mov DWORD PTR [rdi], eax\n");
    } else if (sizeof_lhs == 8) {
        fprintf(g->out, "  mov [rdi], rax\n");
    } else {
        if (expr->op != TokenKind_assign) {
            unimplemented();
        }
        // rax: address of rhs
        // rdi: address of lhs
        // Perform byte-wise copy. Use r10 register as temporary space.
        for (int i = 0; i < sizeof_lhs; ++i) {
            fprintf(g->out, "  mov r10b, %d[rax]\n", i);
            fprintf(g->out, "  mov %d[rdi], r10b\n", i);
        }
    }
}

static void codegen_args(CodeGen* g, AstNode* args) {
    int* required_gp_regs_for_each_arg = calloc(args->as.list->len, sizeof(int));

    int gp_regs = 6;
    for (int i = 0; i < args->as.list->len; ++i) {
        AstNode* arg = &args->as.list->items[i];
        int ty_size = type_sizeof(arg->ty);
        // TODO
        if (arg->ty->kind == TypeKind_array) {
            ty_size = 8;
        }
        int required_gp_regs;
        if (ty_size <= 8) {
            required_gp_regs = 1;
        } else if (ty_size <= 16) {
            required_gp_regs = 2;
        } else {
            required_gp_regs = 0;
        }
        if (required_gp_regs <= gp_regs) {
            gp_regs -= required_gp_regs;
        } else {
            required_gp_regs = 0;
        }
        required_gp_regs_for_each_arg[i] = required_gp_regs;
    }

    // Evaluate arguments in the reverse order (right to left).
    // Arguments passed by stack.
    for (int i = args->as.list->len - 1; i >= 0; --i) {
        AstNode* arg = &args->as.list->items[i];
        if (required_gp_regs_for_each_arg[i] == 0) {
            codegen_expr(g, arg, GenMode_rval);
            int ty_size = type_sizeof(arg->ty);
            if (ty_size <= 8) {
                fprintf(g->out, "  push rax\n");
            } else {
                // Perform byte-wise copy onto stack. Use r10 register as temporary space.
                // NOTE: rsp must be aligned to 8.
                fprintf(g->out, "  sub rsp, %d\n", to_aligned(ty_size, 8));
                for (int i = 0; i < ty_size; ++i) {
                    // Copy a sinle byte from the address that rax points to to the stack via r10 register.
                    fprintf(g->out, "  mov r10b, %d[rax]\n", i);
                    fprintf(g->out, "  mov %d[rsp], r10b\n", i);
                }
            }
        }
    }
    // Arguments passed by registers.
    for (int i = args->as.list->len - 1; i >= 0; --i) {
        AstNode* arg = &args->as.list->items[i];
        if (required_gp_regs_for_each_arg[i] != 0) {
            codegen_expr(g, arg, GenMode_rval);
            if (required_gp_regs_for_each_arg[i] == 1) {
                fprintf(g->out, "  push rax\n");
            } else {
                assert(required_gp_regs_for_each_arg[i] == 2);
                fprintf(g->out, "  push [rax]\n");
                fprintf(g->out, "  push [rax+8]\n");
            }
        }
    }
    // Pop pushed arguments onto registers.
    for (int i = 0, j = 0; i < args->as.list->len; ++i) {
        for (int k = 0; k < required_gp_regs_for_each_arg[i]; ++k) {
            fprintf(g->out, "  pop %s\n", param_reg(j++));
        }
    }
}

static void codegen_func_call(CodeGen* g, FuncCallNode* call) {
    const char* func_name = call->name;

    if (strcmp(func_name, "__ducc_va_start") == 0) {
        fprintf(g->out, "  # __ducc_va_start BEGIN\n");
        AstNode* va_list_args = &call->args->as.list->items[0];
        codegen_expr(g, va_list_args, GenMode_rval);
        fprintf(g->out, "  mov rdi, rax\n");

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

        fprintf(g->out, "  mov rax, 0\n"); // dummy return value
        fprintf(g->out, "  # __ducc_va_start END\n");
        return;
    }

    if (strcmp(func_name, "__ducc_va_arg") == 0) {
        fprintf(g->out, "  # __ducc_va_arg BEGIN\n");

        // Evaluate va_list argument (first argument)
        AstNode* va_list_arg = &call->args->as.list->items[0];
        codegen_expr(g, va_list_arg, GenMode_rval);
        fprintf(g->out, "  mov rdi, rax\n"); // rdi = pointer to va_list

        // Evaluate size argument (second argument)
        AstNode* size_arg = &call->args->as.list->items[1];
        codegen_expr(g, size_arg, GenMode_rval);
        fprintf(g->out, "  mov rsi, rax\n"); // rsi = size

        int label = codegen_new_label(g);

        // Check if gp_offset < 48 (6 registers * 8 bytes)
        fprintf(g->out, "  mov eax, DWORD PTR [rdi]\n"); // eax = gp_offset
        fprintf(g->out, "  cmp eax, 48\n");
        fprintf(g->out, "  jae .Lva_arg_overflow%d\n", label);

        // Fetch from register save area
        fprintf(g->out, "  mov rcx, QWORD PTR [rdi+16]\n"); // rcx = reg_save_area
        fprintf(g->out, "  movsx rdx, eax\n"); // rdx = gp_offset (sign-extended)
        fprintf(g->out, "  add rcx, rdx\n"); // rcx = reg_save_area + gp_offset
        fprintf(g->out, "  add eax, 8\n"); // gp_offset += 8
        fprintf(g->out, "  mov DWORD PTR [rdi], eax\n"); // store updated gp_offset
        fprintf(g->out, "  mov rax, rcx\n"); // return pointer to argument
        fprintf(g->out, "  jmp .Lva_arg_end%d\n", label);

        // Fetch from overflow area (stack)
        fprintf(g->out, ".Lva_arg_overflow%d:\n", label);
        fprintf(g->out, "  mov rcx, QWORD PTR [rdi+8]\n"); // rcx = overflow_arg_area
        fprintf(g->out, "  mov rax, rcx\n"); // return pointer to argument
        fprintf(g->out, "  add rcx, 8\n"); // overflow_arg_area += 8
        fprintf(g->out, "  mov QWORD PTR [rdi+8], rcx\n"); // store updated overflow_arg_area

        fprintf(g->out, ".Lva_arg_end%d:\n", label);
        fprintf(g->out, "  # __ducc_va_arg END\n");
        return;
    }

    AstNode* args = call->args;

    int gp_regs = 6;
    int pass_by_stack_offset = -16;
    for (int i = 0; i < args->as.list->len; ++i) {
        AstNode* arg = &args->as.list->items[i];
        int ty_size = type_sizeof(arg->ty);
        // TODO
        if (arg->ty->kind == TypeKind_array) {
            ty_size = 8;
        }
        int required_gp_regs;
        if (ty_size <= 8) {
            required_gp_regs = 1;
        } else if (ty_size <= 16) {
            required_gp_regs = 2;
        } else {
            required_gp_regs = 0;
        }
        if (required_gp_regs <= gp_regs) {
            gp_regs -= required_gp_regs;
        } else {
            required_gp_regs = 0;
        }
        if (required_gp_regs == 0) {
            pass_by_stack_offset -= to_aligned(ty_size, 8);
        }
    }

    int label = codegen_new_label(g);

    fprintf(g->out, "  mov rax, rsp\n");
    if (-pass_by_stack_offset % 16 != 0) {
        fprintf(g->out, "  add rax, 8\n");
    }
    fprintf(g->out, "  and rax, 15\n");
    fprintf(g->out, "  cmp rax, 0\n");
    fprintf(g->out, "  je .Laligned%d\n", label);

    fprintf(g->out, "  sub rsp, 8\n");
    codegen_args(g, args);
    fprintf(g->out, "  mov rax, 0\n");
    fprintf(g->out, "  call %s\n", func_name);
    fprintf(g->out, "  add rsp, 8\n");

    fprintf(g->out, "  jmp .Lend%d\n", label);
    fprintf(g->out, ".Laligned%d:\n", label);

    codegen_args(g, args);
    fprintf(g->out, "  mov rax, 0\n");
    fprintf(g->out, "  call %s\n", func_name);

    fprintf(g->out, ".Lend%d:\n", label);
    // Pop pass-by-stack arguments.
    fprintf(g->out, "  add rsp, %d\n", -pass_by_stack_offset - 16);
}

static void codegen_lvar(CodeGen* g, LvarNode* lvar, Type* ty, GenMode gen_mode) {
    fprintf(g->out, "  lea rax, %d[rbp]\n", -lvar->stack_offset);
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(g, ty);
    }
}

static void codegen_gvar(CodeGen* g, GvarNode* gvar, Type* ty, GenMode gen_mode) {
    fprintf(g->out, "  lea rax, %s[rip]\n", gvar->name);
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(g, ty);
    }
}

static void codegen_func_ref(CodeGen* g, FuncNode* func) {
    fprintf(g->out, "  lea rax, %s[rip]\n", func->name);
}

static void codegen_composite_expr(CodeGen* g, AstNode* ast) {
    // Standard C does not have composite expression, but ducc internally has.
    for (int i = 0; i < ast->as.list->len; ++i) {
        AstNode* expr = ast->as.list->items + i;
        codegen_expr(g, expr, GenMode_rval);
    }
}

static void codegen_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    if (ast->kind == AstNodeKind_int_expr) {
        codegen_int_expr(g, ast->as.int_expr);
    } else if (ast->kind == AstNodeKind_str_expr) {
        codegen_str_expr(g, ast->as.str_expr);
    } else if (ast->kind == AstNodeKind_unary_expr) {
        codegen_unary_expr(g, ast->as.unary_expr);
    } else if (ast->kind == AstNodeKind_ref_expr) {
        codegen_ref_expr(g, ast->as.ref_expr);
    } else if (ast->kind == AstNodeKind_deref_expr) {
        codegen_deref_expr(g, ast->as.deref_expr, gen_mode);
    } else if (ast->kind == AstNodeKind_cast_expr) {
        codegen_cast_expr(g, ast->as.cast_expr, ast->ty);
    } else if (ast->kind == AstNodeKind_binary_expr) {
        codegen_binary_expr(g, ast->as.binary_expr, gen_mode);
    } else if (ast->kind == AstNodeKind_cond_expr) {
        codegen_cond_expr(g, ast->as.cond_expr, gen_mode);
    } else if (ast->kind == AstNodeKind_logical_expr) {
        codegen_logical_expr(g, ast->as.logical_expr);
    } else if (ast->kind == AstNodeKind_assign_expr) {
        codegen_assign_expr(g, ast->as.assign_expr);
    } else if (ast->kind == AstNodeKind_func_call) {
        codegen_func_call(g, ast->as.func_call);
    } else if (ast->kind == AstNodeKind_lvar) {
        codegen_lvar(g, ast->as.lvar, ast->ty, gen_mode);
    } else if (ast->kind == AstNodeKind_gvar) {
        codegen_gvar(g, ast->as.gvar, ast->ty, gen_mode);
    } else if (ast->kind == AstNodeKind_func) {
        codegen_func_ref(g, ast->as.func);
    } else if (ast->kind == AstNodeKind_list) {
        codegen_composite_expr(g, ast);
    } else {
        unreachable();
    }
}

static void codegen_return_stmt(CodeGen* g, ReturnStmtNode* stmt) {
    if (stmt->expr) {
        codegen_expr(g, stmt->expr, GenMode_rval);
    }
    codegen_func_epilogue(g);
}

static void codegen_if_stmt(CodeGen* g, IfStmtNode* stmt) {
    int label = codegen_new_label(g);

    codegen_expr(g, stmt->cond, GenMode_rval);
    fprintf(g->out, "  cmp rax, 0\n");
    fprintf(g->out, "  je .Lelse%d\n", label);
    codegen_stmt(g, stmt->then);
    fprintf(g->out, "  jmp .Lend%d\n", label);
    fprintf(g->out, ".Lelse%d:\n", label);
    if (stmt->else_) {
        codegen_stmt(g, stmt->else_);
    }
    fprintf(g->out, ".Lend%d:\n", label);
}

static void codegen_for_stmt(CodeGen* g, ForStmtNode* stmt) {
    int label = codegen_new_label(g);
    ++g->loop_labels;
    *g->loop_labels = label;

    if (stmt->init) {
        codegen_expr(g, stmt->init, GenMode_rval);
    }
    fprintf(g->out, ".Lbegin%d:\n", label);
    codegen_expr(g, stmt->cond, GenMode_rval);
    fprintf(g->out, "  cmp rax, 0\n");
    fprintf(g->out, "  je .Lend%d\n", label);
    codegen_stmt(g, stmt->body);
    fprintf(g->out, ".Lcontinue%d:\n", label);
    if (stmt->update) {
        codegen_expr(g, stmt->update, GenMode_rval);
    }
    fprintf(g->out, "  jmp .Lbegin%d\n", label);
    fprintf(g->out, ".Lend%d:\n", label);

    --g->loop_labels;
}

static void codegen_do_while_stmt(CodeGen* g, DoWhileStmtNode* stmt) {
    int label = codegen_new_label(g);
    ++g->loop_labels;
    *g->loop_labels = label;

    fprintf(g->out, ".Lbegin%d:\n", label);
    codegen_stmt(g, stmt->body);
    fprintf(g->out, ".Lcontinue%d:\n", label);
    codegen_expr(g, stmt->cond, GenMode_rval);
    fprintf(g->out, "  cmp rax, 0\n");
    fprintf(g->out, "  je .Lend%d\n", label);
    fprintf(g->out, "  jmp .Lbegin%d\n", label);
    fprintf(g->out, ".Lend%d:\n", label);

    --g->loop_labels;
}

static void codegen_break_stmt(CodeGen* g) {
    if (g->switch_label != -1) {
        fprintf(g->out, "  jmp .Lend%d\n", g->switch_label);
    } else {
        int label = *g->loop_labels;
        fprintf(g->out, "  jmp .Lend%d\n", label);
    }
}

static void codegen_continue_stmt(CodeGen* g) {
    int label = *g->loop_labels;
    fprintf(g->out, "  jmp .Lcontinue%d\n", label);
}

static void codegen_goto_stmt(CodeGen* g, GotoStmtNode* stmt) {
    fprintf(g->out, "  jmp .L%s__%s\n", g->current_func->as.func_def->name, stmt->label);
}

static void codegen_label_stmt(CodeGen* g, LabelStmtNode* stmt) {
    fprintf(g->out, ".L%s__%s:\n", g->current_func->as.func_def->name, stmt->name);
    codegen_stmt(g, stmt->body);
}

// Helper to collect case values from the switch body
static void collect_cases(AstNode* stmt, int* case_values, int* case_labels, int* n_cases) {
    if (!stmt)
        return;

    if (stmt->kind == AstNodeKind_case_label) {
        case_values[*n_cases] = stmt->as.case_label->value;
        case_labels[*n_cases] = *n_cases + 1;
        (*n_cases)++;
        collect_cases(stmt->as.case_label->body, case_values, case_labels, n_cases);
    } else if (stmt->kind == AstNodeKind_default_label) {
        collect_cases(stmt->as.default_label->body, case_values, case_labels, n_cases);
    } else if (stmt->kind == AstNodeKind_list) {
        for (int i = 0; i < stmt->as.list->len; i++) {
            collect_cases(stmt->as.list->items + i, case_values, case_labels, n_cases);
        }
    }
}

static bool codegen_switch_body(CodeGen* g, AstNode* stmt, int* case_values, int* case_labels, int n_cases) {
    if (!stmt)
        return false;

    if (stmt->kind == AstNodeKind_case_label) {
        int value = stmt->as.case_label->value;
        for (int i = 0; i < n_cases; i++) {
            if (case_values[i] == value) {
                fprintf(g->out, ".Lcase%d_%d:\n", g->switch_label, case_labels[i]);
                break;
            }
        }
        codegen_stmt(g, stmt->as.case_label->body);
        return false;
    } else if (stmt->kind == AstNodeKind_default_label) {
        fprintf(g->out, ".Ldefault%d:\n", g->switch_label);
        codegen_stmt(g, stmt->as.default_label->body);
        return true;
    } else if (stmt->kind == AstNodeKind_list) {
        bool default_label_emitted = false;
        for (int i = 0; i < stmt->as.list->len; i++) {
            default_label_emitted |=
                codegen_switch_body(g, stmt->as.list->items + i, case_values, case_labels, n_cases);
        }
        return default_label_emitted;
    } else {
        codegen_stmt(g, stmt);
        return false;
    }
}

static void codegen_switch_stmt(CodeGen* g, SwitchStmtNode* stmt) {
    int switch_label = codegen_new_label(g);
    int prev_switch_label = g->switch_label;
    g->switch_label = switch_label;

    // Collect all case values and assign labels
    int case_values[256];
    int case_labels[256];
    int n_cases = 0;
    collect_cases(stmt->body, case_values, case_labels, &n_cases);

    // Generate jump instructions.
    codegen_expr(g, stmt->expr, GenMode_rval);
    for (int i = 0; i < n_cases; i++) {
        fprintf(g->out, "  cmp rax, %d\n", case_values[i]);
        fprintf(g->out, "  je .Lcase%d_%d\n", switch_label, case_labels[i]);
    }
    fprintf(g->out, "  jmp .Ldefault%d\n", switch_label);

    // Generate the switch body with labels.
    bool default_label_emitted = codegen_switch_body(g, stmt->body, case_values, case_labels, n_cases);

    if (!default_label_emitted) {
        fprintf(g->out, ".Ldefault%d:\n", switch_label);
    }
    fprintf(g->out, ".Lend%d:\n", switch_label);

    g->switch_label = prev_switch_label;
}

static void codegen_expr_stmt(CodeGen* g, ExprStmtNode* stmt) {
    codegen_expr(g, stmt->expr, GenMode_rval);
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
    } else if (ast->kind == AstNodeKind_switch_stmt) {
        codegen_switch_stmt(g, ast->as.switch_stmt);
    } else if (ast->kind == AstNodeKind_for_stmt) {
        codegen_for_stmt(g, ast->as.for_stmt);
    } else if (ast->kind == AstNodeKind_do_while_stmt) {
        codegen_do_while_stmt(g, ast->as.do_while_stmt);
    } else if (ast->kind == AstNodeKind_break_stmt) {
        codegen_break_stmt(g);
    } else if (ast->kind == AstNodeKind_continue_stmt) {
        codegen_continue_stmt(g);
    } else if (ast->kind == AstNodeKind_goto_stmt) {
        codegen_goto_stmt(g, ast->as.goto_stmt);
    } else if (ast->kind == AstNodeKind_label_stmt) {
        codegen_label_stmt(g, ast->as.label_stmt);
    } else if (ast->kind == AstNodeKind_expr_stmt) {
        codegen_expr_stmt(g, ast->as.expr_stmt);
    } else if (ast->kind == AstNodeKind_lvar_decl) {
        // Do nothing.
    } else if (ast->kind == AstNodeKind_nop) {
        // Do nothing.
    } else if (ast->kind == AstNodeKind_case_label || ast->kind == AstNodeKind_default_label) {
        // They are handled by codegen_switch_stmt().
        unreachable();
    } else {
        unreachable();
    }
}

static void codegen_func(CodeGen* g, AstNode* ast) {
    g->current_func = ast;

    if (ast->ty->storage_class != StorageClass_static) {
        fprintf(g->out, ".globl %s\n", ast->as.func_def->name);
    }
    fprintf(g->out, "%s:\n", ast->as.func_def->name);

    codegen_func_prologue(g, ast->as.func_def);
    codegen_stmt(g, ast->as.func_def->body);
    if (strcmp(ast->as.func_def->name, "main") == 0) {
        // C99: 5.1.2.2.3
        fprintf(g->out, "  mov rax, 0\n");
    }
    codegen_func_epilogue(g);

    fprintf(g->out, "\n");
    g->current_func = NULL;
}

static void codegen_global_var(CodeGen* g, AstNode* var) {
    fprintf(g->out, "  %s:\n", var->as.gvar_decl->name);
    if (!var->as.gvar_decl->expr) {
        fprintf(g->out, "    .zero %d\n", type_sizeof(var->ty));
        return;
    }

    if (var->ty->kind == TypeKind_array && var->as.gvar_decl->expr->kind == AstNodeKind_str_expr) {
        const char* str = g->prog->str_literals[var->as.gvar_decl->expr->as.str_expr->idx - 1];
        fprintf(g->out, "    .string \"%s\"\n", str);
        return;
    }

    InitData* data = eval_init_expr(var->as.gvar_decl->expr, var->ty);

    for (size_t i = 0; i < data->len; ++i) {
        InitDataBlock* block = &data->blocks[i];
        if (block->kind == InitDataBlockKind_addr) {
            fprintf(g->out, "    .quad %s\n", block->as.addr.label);
        } else {
            for (size_t j = 0; j < block->as.bytes.len; ++j) {
                fprintf(g->out, "    .byte %d\n", block->as.bytes.buf[j]);
            }
        }
    }
}

void codegen(Program* prog, FILE* out) {
    CodeGen* g = codegen_new(prog, out);

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
    for (int i = 0; i < prog->vars->as.list->len; ++i) {
        codegen_global_var(g, &prog->vars->as.list->items[i]);
    }

    fprintf(g->out, ".text\n\n");
    for (int i = 0; i < prog->funcs->as.list->len; ++i) {
        AstNode* func = &prog->funcs->as.list->items[i];
        codegen_func(g, func);
    }
}
