enum GenMode {
    GenMode_lval,
    GenMode_rval,
};
typedef enum GenMode GenMode;

struct CodeGen {
    int next_label;
    int* loop_labels;
};
typedef struct CodeGen CodeGen;

CodeGen* codegen_new() {
    CodeGen* g = calloc(1, sizeof(CodeGen));
    g->next_label = 1;
    g->loop_labels = calloc(1024, sizeof(int));
    return g;
}

int codegen_new_label(CodeGen* g) {
    int new_label = g->next_label;
    ++g->next_label;
    return new_label;
}

void codegen_expr(CodeGen* g, AstNode* ast, GenMode gen_mode);
void codegen_stmt(CodeGen* g, AstNode* ast);

const char* param_reg(int n) {
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

void codegen_func_prologue(CodeGen* g, AstNode* ast) {
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    int i;
    for (i = 0; i < ast->node_params->node_len; ++i) {
        printf("  push %s\n", param_reg(i));
    }
    printf("  sub rsp, %d\n", 8 * LVAR_MAX);
}

void codegen_func_epilogue(CodeGen* g, AstNode* ast) {
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

void codegen_int_expr(CodeGen* g, AstNode* ast) {
    printf("  push %d\n", ast->node_int_value);
}

void codegen_str_expr(CodeGen* g, AstNode* ast) {
    printf("  mov rax, OFFSET FLAG:.Lstr__%d\n", ast->node_idx);
    printf("  push rax\n");
}

void codegen_unary_expr(CodeGen* g, AstNode* ast) {
    codegen_expr(g, ast->node_operand, GenMode_rval);
    if (ast->node_op == TokenKind_not) {
        printf("  pop rax\n");
        printf("  mov rdi, 0\n");
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        printf("  push rax\n");
    } else {
        unreachable();
    }
}

void codegen_ref_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    codegen_expr(g, ast->node_operand, GenMode_lval);
}

// "reg" stores the address of the expression to be pushed.
void codegen_push_expr(const char* reg, int size) {
    if (size == 1) {
        printf("  movsx %s, BYTE PTR [%s]\n", reg, reg);
        printf("  push %s\n", reg);
    } else if (size == 2) {
        printf("  movsx %s, WORD PTR [%s]\n", reg, reg);
        printf("  push %s\n", reg);
    } else if (size == 4) {
        printf("  movsxd %s, DWORD PTR [%s]\n", reg, reg);
        printf("  push %s\n", reg);
    } else if (size == 8) {
        printf("  mov %s, [%s]\n", reg, reg);
        printf("  push %s\n", reg);
    } else {
        // Perform bitwise copy. Use r10 register as temporary space.
        // Note: rsp must be aligned to 8.
        printf("  sub rsp, %d\n", to_aligned(size, 8));
        int i;
        for (i = 0; i < size; ++i) {
            // Copy a sinle byte from the address that "reg" points to to the stack via r10 register.
            printf("  mov r10b, [%s+%d]\n", reg, i);
            printf("  mov [rsp+%d], r10b\n", i);
        }
    }
}

void codegen_lval2rval(Type* ty) {
    if (ty->kind == TypeKind_array) {
        return;
    }

    printf("  pop rax\n");
    codegen_push_expr("rax", type_sizeof(ty));
}

void codegen_deref_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    codegen_expr(g, ast->node_operand, GenMode_rval);
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(ast->node_operand->ty->base);
    }
}

void codegen_logical_expr(CodeGen* g, AstNode* ast) {
    int label = codegen_new_label(g);

    if (ast->node_op == TokenKind_andand) {
        codegen_expr(g, ast->node_lhs, GenMode_rval);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lelse%d\n", label);
        codegen_expr(g, ast->node_rhs, GenMode_rval);
        printf("  jmp .Lend%d\n", label);
        printf(".Lelse%d:\n", label);
        printf("  push 0\n");
        printf(".Lend%d:\n", label);
    } else {
        codegen_expr(g, ast->node_lhs, GenMode_rval);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lelse%d\n", label);
        printf("  push 1\n");
        printf("  jmp .Lend%d\n", label);
        printf(".Lelse%d:\n", label);
        codegen_expr(g, ast->node_rhs, GenMode_rval);
        printf(".Lend%d:\n", label);
    }
}

void codegen_binary_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    codegen_expr(g, ast->node_lhs, gen_mode);
    codegen_expr(g, ast->node_rhs, gen_mode);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    if (ast->node_op == TokenKind_plus) {
        printf("  add rax, rdi\n");
    } else if (ast->node_op == TokenKind_minus) {
        printf("  sub rax, rdi\n");
    } else if (ast->node_op == TokenKind_star) {
        printf("  imul rax, rdi\n");
    } else if (ast->node_op == TokenKind_slash) {
        printf("  cqo\n");
        printf("  idiv rdi\n");
    } else if (ast->node_op == TokenKind_percent) {
        printf("  cqo\n");
        printf("  idiv rdi\n");
        printf("  mov rax, rdx\n");
    } else if (ast->node_op == TokenKind_or) {
        printf("  or rax, rdi\n");
    } else if (ast->node_op == TokenKind_eq) {
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
    } else if (ast->node_op == TokenKind_ne) {
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
    } else if (ast->node_op == TokenKind_lt) {
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
    } else if (ast->node_op == TokenKind_le) {
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
    } else {
        unreachable();
    }
    printf("  push rax\n");
}

void codegen_assign_expr(CodeGen* g, AstNode* ast) {
    int sizeof_lhs = type_sizeof(ast->node_lhs->ty);
    int sizeof_rhs = type_sizeof(ast->node_rhs->ty);

    codegen_expr(g, ast->node_lhs, GenMode_lval);
    codegen_expr(g, ast->node_rhs, GenMode_rval);
    if (ast->node_op == TokenKind_assign) {
    } else if (ast->node_op == TokenKind_assign_add) {
        printf("  pop rdi\n");
        printf("  push [rsp]\n");
        codegen_lval2rval(ast->node_lhs->ty);
        printf("  pop rax\n");
        printf("  add rax, rdi\n");
        printf("  push rax\n");
    } else if (ast->node_op == TokenKind_assign_sub) {
        printf("  pop rdi\n");
        printf("  push [rsp]\n");
        codegen_lval2rval(ast->node_lhs->ty);
        printf("  pop rax\n");
        printf("  sub rax, rdi\n");
        printf("  push rax\n");
    } else {
        unreachable();
    }
    if (sizeof_lhs == 1) {
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov BYTE PTR [rax], dil\n");
        printf("  push rdi\n");
    } else if (sizeof_lhs == 2) {
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov WORD PTR [rax], di\n");
        printf("  push rdi\n");
    } else if (sizeof_lhs == 4) {
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov DWORD PTR [rax], edi\n");
        printf("  push rdi\n");
    } else if (sizeof_lhs == 8) {
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
    } else {
        if (ast->node_op != TokenKind_assign) {
            unimplemented();
        }
        // Note: rsp must be aligned to 8.
        int aligned_size = to_aligned(sizeof_lhs, 8);
        printf("  mov rax, [rsp+%d]\n", aligned_size);
        // Perform bitwise copy. Use r10 register as temporary space.
        int i;
        for (i = 0; i < aligned_size; ++i) {
            // Copy a sinle byte from the stack to the address that rax points to via r10 register.
            printf("  mov r10b, [rsp+%d]\n", i);
            printf("  mov [rax+%d], r10b\n", i);
        }
        // Pop the RHS value and the LHS address.
        printf("  add rsp, %d\n", aligned_size + 8);
        // TODO: dummy
        printf("  push 0\n");
    }
}

void codegen_func_call(CodeGen* g, AstNode* ast) {
    String* func_name = &ast->name;
    int i;

    if (string_equals_cstr(func_name, "va_start")) {
        printf("  # va_start BEGIN\n");
        for (i = 0; i < 6; ++i) {
            printf("  mov rax, %s\n", param_reg(i));
            printf("  mov [rbp-%d], rax\n", 8 + (LVAR_MAX - 4 - i) * 8);
        }
        AstNode* va_list_args = ast->node_args->node_items;
        codegen_expr(g, va_list_args, GenMode_lval);
        printf("  pop rdi\n");
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", 8 + (LVAR_MAX - 1) * 8);
        printf("  mov [rdi], rax\n");
        printf("  mov DWORD PTR [rax], 8\n");
        printf("  mov DWORD PTR [rax+4], 0\n");
        printf("  mov QWORD PTR [rax+8], 0\n");
        printf("  mov rdi, rbp\n");
        printf("  sub rdi, %d\n", 8 + (LVAR_MAX - 4) * 8);
        printf("  mov QWORD PTR [rax+16], rdi\n");
        printf("  # va_start END\n");
        return;
    }

    AstNode* args = ast->node_args;
    for (i = 0; i < args->node_len; ++i) {
        AstNode* arg = args->node_items + i;
        codegen_expr(g, arg, GenMode_rval);
    }
    for (i = args->node_len - 1; i >= 0; --i) {
        printf("  pop %s\n", param_reg(i));
    }

    int label = codegen_new_label(g);

    printf("  mov rax, rsp\n");
    printf("  and rax, 15\n");
    printf("  cmp rax, 0\n");
    printf("  je .Laligned%d\n", label);

    printf("  mov rax, 0\n");
    printf("  sub rsp, 8\n");
    printf("  call %.*s\n", func_name->len, func_name->data);
    printf("  add rsp, 8\n");
    printf("  push rax\n");

    printf("  jmp .Lend%d\n", label);
    printf(".Laligned%d:\n", label);

    printf("  mov rax, 0\n");
    printf("  call %.*s\n", func_name->len, func_name->data);
    printf("  push rax\n");

    printf(".Lend%d:\n", label);
}

void codegen_lvar(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", ast->node_stack_offset);
    printf("  push rax\n");
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(ast->ty);
    }
}

void codegen_gvar(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    if (gen_mode == GenMode_lval) {
        unimplemented();
    }
    if (ast->ty->kind != TypeKind_ptr) {
        unimplemented();
    }
    printf("  mov rax, QWORD PTR %.*s[rip]\n", ast->name.len, ast->name.data);
    printf("  push rax\n");
}

void codegen_composite_expr(CodeGen* g, AstNode* ast) {
    // Standard C does not have composite expression, but ducc internally has.
    int i;
    for (i = 0; i < ast->node_len; ++i) {
        AstNode* expr = ast->node_items + i;
        codegen_expr(g, expr, GenMode_rval);
        if (i != ast->node_len - 1) {
            // TODO: the expression on the stack can be more than 8 bytes.
            printf("  pop rax\n");
        }
    }
}

void codegen_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
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
    } else if (ast->kind == AstNodeKind_binary_expr) {
        codegen_binary_expr(g, ast, gen_mode);
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

void codegen_return_stmt(CodeGen* g, AstNode* ast) {
    if (ast->node_expr) {
        codegen_expr(g, ast->node_expr, GenMode_rval);
        printf("  pop rax\n");
    }
    codegen_func_epilogue(g, ast);
}

void codegen_if_stmt(CodeGen* g, AstNode* ast) {
    int label = codegen_new_label(g);

    codegen_expr(g, ast->node_cond, GenMode_rval);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%d\n", label);
    codegen_stmt(g, ast->node_then);
    printf("  jmp .Lend%d\n", label);
    printf(".Lelse%d:\n", label);
    if (ast->node_else) {
        codegen_stmt(g, ast->node_else);
    }
    printf(".Lend%d:\n", label);
}

void codegen_for_stmt(CodeGen* g, AstNode* ast) {
    int label = codegen_new_label(g);
    ++g->loop_labels;
    *g->loop_labels = label;

    if (ast->node_init) {
        codegen_expr(g, ast->node_init, GenMode_rval);
        printf("  pop rax\n");
    }
    printf(".Lbegin%d:\n", label);
    codegen_expr(g, ast->node_cond, GenMode_rval);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", label);
    codegen_stmt(g, ast->node_body);
    printf(".Lcontinue%d:\n", label);
    if (ast->node_update) {
        codegen_expr(g, ast->node_update, GenMode_rval);
        printf("  pop rax\n");
    }
    printf("  jmp .Lbegin%d\n", label);
    printf(".Lend%d:\n", label);

    --g->loop_labels;
}

void codegen_do_while_stmt(CodeGen* g, AstNode* ast) {
    int label = codegen_new_label(g);
    ++g->loop_labels;
    *g->loop_labels = label;

    printf(".Lbegin%d:\n", label);
    codegen_stmt(g, ast->node_body);
    printf(".Lcontinue%d:\n", label);
    codegen_expr(g, ast->node_cond, GenMode_rval);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", label);
    printf("  jmp .Lbegin%d\n", label);
    printf(".Lend%d:\n", label);

    --g->loop_labels;
}

void codegen_break_stmt(CodeGen* g, AstNode* ast) {
    int label = *g->loop_labels;
    printf("  jmp .Lend%d\n", label);
}

void codegen_continue_stmt(CodeGen* g, AstNode* ast) {
    int label = *g->loop_labels;
    printf("  jmp .Lcontinue%d\n", label);
}

void codegen_expr_stmt(CodeGen* g, AstNode* ast) {
    codegen_expr(g, ast->node_expr, GenMode_rval);
    // TODO: the expression on the stack can be more than 8 bytes.
    printf("  pop rax\n");
}

void codegen_var_decl(CodeGen* g, AstNode* ast) {
}

void codegen_nop(CodeGen* g, AstNode* ast) {
}

void codegen_block_stmt(CodeGen* g, AstNode* ast) {
    int i;
    for (i = 0; i < ast->node_len; ++i) {
        AstNode* stmt = ast->node_items + i;
        codegen_stmt(g, stmt);
    }
}

void codegen_stmt(CodeGen* g, AstNode* ast) {
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

void codegen_func(CodeGen* g, AstNode* ast) {
    printf("%.*s:\n", ast->name.len, ast->name.data);

    codegen_func_prologue(g, ast);
    codegen_stmt(g, ast->node_body);
    if (string_equals_cstr(&ast->name, "main")) {
        // C99: 5.1.2.2.3
        printf("  mov rax, 0\n");
    }
    codegen_func_epilogue(g, ast);

    printf("\n");
}

void codegen(Program* prog) {
    CodeGen* g = codegen_new();

    printf(".intel_syntax noprefix\n\n");

    int i;
    for (i = 0; prog->str_literals[i]; ++i) {
        printf(".Lstr__%d:\n", i + 1);
        printf("  .string \"%s\"\n\n", prog->str_literals[i]);
    }

    printf(".globl main\n\n");

    for (i = 0; i < prog->funcs->node_len; ++i) {
        AstNode* func = prog->funcs->node_items + i;
        codegen_func(g, func);
    }
}
