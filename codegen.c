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

void codegen_lval2rval(Type* ty) {
    int size = type_sizeof(ty);

    printf("  pop rax\n");
    if (size == 1) {
        printf("  movsx rax, BYTE PTR [rax]\n");
    } else if (size == 4) {
        printf("  movsxd rax, DWORD PTR [rax]\n");
    } else {
        printf("  mov rax, [rax]\n");
    }
    printf("  push rax\n");
}

void codegen_deref_expr(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    codegen_expr(g, ast->node_operand, GenMode_rval);
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(ast->node_operand->ty->to);
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
    printf("  pop rdi\n");
    printf("  pop rax\n");
    if (type_sizeof(ast->node_lhs->ty) == 1) {
        printf("  mov BYTE PTR [rax], dil\n");
    } else if (type_sizeof(ast->node_lhs->ty) == 4) {
        printf("  mov DWORD PTR [rax], edi\n");
    } else {
        printf("  mov [rax], rdi\n");
    }
    printf("  push rdi\n");
}

void codegen_func_call(CodeGen* g, AstNode* ast) {
    String* func_name = &ast->name;
    AstNode* args = ast->node_args;
    int i;
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
    int offset = 8 + ast->node_idx * 8;
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", offset);
    printf("  push rax\n");
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(ast->ty);
    }
}

void codegen_gvar(CodeGen* g, AstNode* ast, GenMode gen_mode) {
    if (gen_mode == GenMode_lval) {
        fatal_error("unimplemented");
    }
    if (ast->ty->kind != TypeKind_ptr) {
        fatal_error("unimplemented");
    }
    printf("  mov rax, QWORD PTR %.*s[rip]\n", ast->name.len, ast->name.data);
    printf("  push rax\n");
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
    printf("  pop rax\n");
}

void codegen_var_decl(CodeGen* g, AstNode* ast) {
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
    } else {
        unreachable();
    }
}

void codegen_func(CodeGen* g, AstNode* ast) {
    printf("%.*s:\n", ast->name.len, ast->name.data);

    codegen_func_prologue(g, ast);
    codegen_stmt(g, ast->node_body);
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
