#define LVAR_MAX 32

struct LocalVar {
    String name;
    Type* ty;
    int stack_offset;
};
typedef struct LocalVar LocalVar;

struct Scope {
    struct Scope* outer;
    String* lvar_names;
    int* lvar_indices;
};
typedef struct Scope Scope;

struct GlobalVar {
    String name;
    Type* ty;
};
typedef struct GlobalVar GlobalVar;

struct Func {
    String name;
    Type* ty;
};
typedef struct Func Func;

struct Parser {
    TokenArray* tokens;
    int pos;
    LocalVar* lvars;
    int n_lvars;
    Scope* scope;
    GlobalVar* gvars;
    int n_gvars;
    Func* funcs;
    int n_funcs;
    AstNode* structs;
    int n_structs;
    AstNode* unions;
    int n_unions;
    AstNode* enums;
    int n_enums;
    AstNode* typedefs;
    int n_typedefs;
    char** str_literals;
    int n_str_literals;
};
typedef struct Parser Parser;

Parser* parser_new(TokenArray* tokens) {
    Parser* p = calloc(1, sizeof(Parser));
    p->tokens = tokens;
    p->gvars = calloc(128, sizeof(GlobalVar));
    p->funcs = calloc(256, sizeof(Func));
    p->structs = calloc(64, sizeof(AstNode));
    p->unions = calloc(64, sizeof(AstNode));
    p->enums = calloc(16, sizeof(AstNode));
    p->typedefs = calloc(64, sizeof(AstNode));
    p->str_literals = calloc(1024, sizeof(char*));

    p->funcs[p->n_funcs].name.data = "va_start";
    p->funcs[p->n_funcs].name.len = strlen("va_start");
    p->funcs[p->n_funcs].ty = calloc(1, sizeof(Type));
    p->funcs[p->n_funcs].ty->kind = TypeKind_void;
    ++p->n_funcs;

    return p;
}

Token* peek_token(Parser* p) {
    return &p->tokens->data[p->pos];
}

Token* next_token(Parser* p) {
    return &p->tokens->data[p->pos++];
}

BOOL eof(Parser* p) {
    return peek_token(p)->kind != TokenKind_eof;
}

Token* expect(Parser* p, TokenKind expected) {
    Token* t = next_token(p);
    if (t->kind == expected) {
        return t;
    }
    fatal_error("%s:%d: expected '%s', but got '%s'", t->loc.filename, t->loc.line, token_kind_stringify(expected),
                token_stringify(t));
}

int find_lvar_in_scope(Parser* p, Scope* scope, const String* name) {
    for (int i = 0; i < LVAR_MAX; ++i) {
        if (string_equals(&scope->lvar_names[i], name)) {
            return scope->lvar_indices[i];
        }
    }
    return -1;
}

int find_lvar_in_current_scope(Parser* p, const String* name) {
    return find_lvar_in_scope(p, p->scope, name);
}

int find_lvar(Parser* p, const String* name) {
    Scope* scope = p->scope;
    while (scope) {
        int idx = find_lvar_in_scope(p, scope, name);
        if (idx != -1)
            return idx;
        scope = scope->outer;
    }
    return -1;
}

int calc_stack_offset(Parser* p, Type* ty, BOOL is_param) {
    int align;
    if (is_param) {
        if (8 < type_sizeof(ty) || 8 < type_alignof(ty)) {
            fatal_error("too large");
        }
        align = 8;
    } else {
        align = type_alignof(ty);
    }

    int offset;
    if (p->n_lvars == 0) {
        offset = 0;
    } else {
        offset = p->lvars[p->n_lvars - 1].stack_offset;
    }

    offset += type_sizeof(ty);
    return to_aligned(offset, align);
}

int add_lvar(Parser* p, String* name, Type* ty, BOOL is_param) {
    int stack_offset = calc_stack_offset(p, ty, is_param);
    p->lvars[p->n_lvars].name = *name;
    p->lvars[p->n_lvars].ty = ty;
    p->lvars[p->n_lvars].stack_offset = stack_offset;
    for (int i = 0; i < LVAR_MAX; ++i) {
        if (p->scope->lvar_names[i].len == 0) {
            p->scope->lvar_names[i] = *name;
            p->scope->lvar_indices[i] = p->n_lvars;
            break;
        }
    }
    ++p->n_lvars;
    return stack_offset;
}

String* generate_temporary_lvar_name(Parser* p) {
    String* ret = calloc(1, sizeof(String));
    ret->data = calloc(256, sizeof(char));
    for (int i = 1;; ++i) {
        ret->len = sprintf(ret->data, "__%d", i);
        if (find_lvar(p, ret) == -1) {
            return ret;
        }
    }
}

AstNode* generate_temporary_lvar(Parser* p, Type* ty) {
    String* name = generate_temporary_lvar_name(p);
    int stack_offset = add_lvar(p, name, ty, FALSE);
    AstNode* lvar = ast_new(AstNodeKind_lvar);
    lvar->name = *name;
    lvar->node_stack_offset = stack_offset;
    lvar->ty = ty;
    return lvar;
}

int find_gvar(Parser* p, const String* name) {
    for (int i = 0; i < p->n_gvars; ++i) {
        if (string_equals(&p->gvars[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int find_func(Parser* p, const String* name) {
    for (int i = 0; i < p->n_funcs; ++i) {
        if (string_equals(&p->funcs[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int find_struct(Parser* p, const String* name) {
    for (int i = 0; i < p->n_structs; ++i) {
        if (string_equals(&p->structs[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int find_union(Parser* p, const String* name) {
    for (int i = 0; i < p->n_unions; ++i) {
        if (string_equals(&p->unions[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int find_enum(Parser* p, const String* name) {
    for (int i = 0; i < p->n_enums; ++i) {
        if (string_equals(&p->enums[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int find_enum_member(Parser* p, const String* name) {
    for (int i = 0; i < p->n_enums; ++i) {
        for (int j = 0; j < p->enums[i].node_members->node_len; ++j) {
            if (string_equals(&p->enums[i].node_members->node_items[j].name, name)) {
                return i * 1000 + j;
            }
        }
    }
    return -1;
}

int find_typedef(Parser* p, const String* name) {
    for (int i = 0; i < p->n_typedefs; ++i) {
        if (string_equals(&p->typedefs[i].name, name)) {
            return i;
        }
    }
    return -1;
}

void enter_scope(Parser* p) {
    Scope* outer_scope = p->scope;
    p->scope = calloc(1, sizeof(Scope));
    p->scope->outer = outer_scope;
    p->scope->lvar_names = calloc(LVAR_MAX, sizeof(String));
    p->scope->lvar_indices = calloc(LVAR_MAX, sizeof(int));
}

void leave_scope(Parser* p) {
    p->scope = p->scope->outer;
}

void enter_func(Parser* p) {
    p->lvars = calloc(LVAR_MAX, sizeof(LocalVar));
    p->n_lvars = 0;
    enter_scope(p);
}

void leave_func(Parser* p) {
    leave_scope(p);
}

AstNode* parse_assignment_expr(Parser* p);
AstNode* parse_expr(Parser* p);
AstNode* parse_stmt(Parser* p);

String* parse_ident(Parser* p) {
    return &expect(p, TokenKind_ident)->raw;
}

int register_str_literal(Parser* p, char* s) {
    p->str_literals[p->n_str_literals] = s;
    ++p->n_str_literals;
    return p->n_str_literals;
}

AstNode* parse_primary_expr(Parser* p) {
    Token* t = next_token(p);
    if (t->kind == TokenKind_literal_int) {
        return ast_new_int(atoi(string_to_cstr(&t->raw)));
    } else if (t->kind == TokenKind_literal_str) {
        AstNode* e = ast_new(AstNodeKind_str_expr);
        e->node_idx = register_str_literal(p, string_to_cstr(&t->raw));
        e->ty = type_new_static_string(t->raw.len);
        return e;
    } else if (t->kind == TokenKind_paren_l) {
        AstNode* e = parse_expr(p);
        expect(p, TokenKind_paren_r);
        return e;
    } else if (t->kind == TokenKind_ident || t->kind == TokenKind_va_start) {
        String* name = &t->raw;

        if (peek_token(p)->kind == TokenKind_paren_l) {
            AstNode* e = ast_new(AstNodeKind_func_call);
            int func_idx = find_func(p, name);
            if (func_idx == -1) {
                fatal_error("undefined function: %.*s", name->len, name->data);
            }
            e->name = *name;
            e->ty = p->funcs[func_idx].ty;
            return e;
        }

        int lvar_idx = find_lvar(p, name);
        if (lvar_idx == -1) {
            int gvar_idx = find_gvar(p, name);
            if (gvar_idx == -1) {
                int enum_member_idx = find_enum_member(p, name);
                if (enum_member_idx == -1) {
                    fatal_error("undefined variable: %.*s", name->len, name->data);
                }
                int enum_idx = enum_member_idx / 1000;
                int n = enum_member_idx % 1000;
                AstNode* e = ast_new_int(p->enums[enum_idx].node_members->node_items[n].node_int_value);
                e->ty = type_new(TypeKind_enum);
                e->ty->def = p->enums + enum_idx;
                return e;
            }
            AstNode* e = ast_new(AstNodeKind_gvar);
            e->name = *name;
            e->ty = p->gvars[gvar_idx].ty;
            return e;
        }

        AstNode* e = ast_new(AstNodeKind_lvar);
        e->name = *name;
        e->node_stack_offset = p->lvars[lvar_idx].stack_offset;
        e->ty = p->lvars[lvar_idx].ty;
        return e;
    } else {
        fatal_error("%s:%d: expected an expression, but got '%s'", t->loc.filename, t->loc.line, token_stringify(t));
    }
}

AstNode* parse_arg_list(Parser* p) {
    AstNode* list = ast_new_list(6);
    while (peek_token(p)->kind != TokenKind_paren_r) {
        AstNode* arg = parse_assignment_expr(p);
        ast_append(list, arg);
        if (peek_token(p)->kind == TokenKind_comma) {
            next_token(p);
        } else {
            break;
        }
    }
    if (list->node_len > 6) {
        fatal_error("too many arguments");
    }
    return list;
}

// e++
// tmp1 = &e; tmp2 = *tmp1; *tmp1 += 1; tmp2
// e--
// tmp1 = &e; tmp2 = *tmp1; *tmp1 -= 1; tmp2
AstNode* create_new_postfix_inc_or_dec(Parser* p, AstNode* e, TokenKind op) {
    AstNode* tmp1_lvar = generate_temporary_lvar(p, type_new_ptr(e->ty));
    AstNode* tmp2_lvar = generate_temporary_lvar(p, e->ty);

    AstNode* expr1 = ast_new_assign_expr(TokenKind_assign, tmp1_lvar, ast_new_ref_expr(e));
    AstNode* expr2 = ast_new_assign_expr(TokenKind_assign, tmp2_lvar, ast_new_deref_expr(tmp1_lvar));
    AstNode* expr3;
    if (op == TokenKind_plusplus) {
        expr3 = ast_new_assign_add_expr(ast_new_deref_expr(tmp1_lvar), ast_new_int(1));
    } else {
        expr3 = ast_new_assign_sub_expr(ast_new_deref_expr(tmp1_lvar), ast_new_int(1));
    }
    AstNode* expr4 = tmp2_lvar;

    AstNode* ret = ast_new_list(4);
    ast_append(ret, expr1);
    ast_append(ret, expr2);
    ast_append(ret, expr3);
    ast_append(ret, expr4);
    ret->ty = expr4->ty;
    return ret;
}

AstNode* parse_postfix_expr(Parser* p) {
    AstNode* ret = parse_primary_expr(p);
    while (1) {
        TokenKind tk = peek_token(p)->kind;
        if (tk == TokenKind_paren_l) {
            next_token(p);
            AstNode* args = parse_arg_list(p);
            expect(p, TokenKind_paren_r);
            ret->node_args = args;
        } else if (tk == TokenKind_bracket_l) {
            next_token(p);
            AstNode* idx = parse_expr(p);
            expect(p, TokenKind_bracket_r);
            idx = ast_new_binary_expr(TokenKind_star, idx, ast_new_int(type_sizeof(ret->ty->base)));
            ret = ast_new_deref_expr(ast_new_binary_expr(TokenKind_plus, ret, idx));
        } else if (tk == TokenKind_dot) {
            next_token(p);
            String* name = parse_ident(p);
            ret = ast_new_member_access_expr(ast_new_ref_expr(ret), name);
        } else if (tk == TokenKind_arrow) {
            next_token(p);
            String* name = parse_ident(p);
            ret = ast_new_member_access_expr(ret, name);
        } else if (tk == TokenKind_plusplus) {
            next_token(p);
            ret = create_new_postfix_inc_or_dec(p, ret, tk);
        } else if (tk == TokenKind_minusminus) {
            next_token(p);
            ret = create_new_postfix_inc_or_dec(p, ret, tk);
        } else {
            break;
        }
    }
    return ret;
}

BOOL is_type_token(Parser* p, Token* token) {
    if (token->kind == TokenKind_keyword_int || token->kind == TokenKind_keyword_short ||
        token->kind == TokenKind_keyword_long || token->kind == TokenKind_keyword_char ||
        token->kind == TokenKind_keyword_void || token->kind == TokenKind_keyword_enum ||
        token->kind == TokenKind_keyword_struct || token->kind == TokenKind_keyword_union ||
        token->kind == TokenKind_keyword_const) {
        return TRUE;
    }
    if (token->kind != TokenKind_ident) {
        return FALSE;
    }
    return find_typedef(p, &token->raw) != -1;
}

Type* parse_type(Parser* p) {
    Token* t = next_token(p);
    if (t->kind == TokenKind_keyword_const) {
        t = next_token(p);
    }
    if (!is_type_token(p, t)) {
        fatal_error("%s:%d: parse_type: expected type, but got '%s'", t->loc.filename, t->loc.line, token_stringify(t));
    }
    Type* ty;
    if (t->kind == TokenKind_ident) {
        int typedef_idx = find_typedef(p, &t->raw);
        if (typedef_idx == -1) {
            fatal_error("parse_type: unknown typedef, %.*s", t->raw.len, t->raw.data);
        }
        ty = p->typedefs[typedef_idx].ty;
    } else {
        ty = type_new(TypeKind_unknown);
        if (t->kind == TokenKind_keyword_char) {
            ty->kind = TypeKind_char;
        } else if (t->kind == TokenKind_keyword_short) {
            ty->kind = TypeKind_short;
        } else if (t->kind == TokenKind_keyword_int) {
            ty->kind = TypeKind_int;
        } else if (t->kind == TokenKind_keyword_long) {
            ty->kind = TypeKind_long;
        } else if (t->kind == TokenKind_keyword_void) {
            ty->kind = TypeKind_void;
        } else if (t->kind == TokenKind_keyword_enum) {
            ty->kind = TypeKind_enum;
            String* name = parse_ident(p);
            int enum_idx = find_enum(p, name);
            if (enum_idx == -1) {
                fatal_error("parse_type: unknown enum, %.*s", name->len, name->data);
            }
            ty->def = p->enums + enum_idx;
        } else if (t->kind == TokenKind_keyword_struct) {
            ty->kind = TypeKind_struct;
            String* name = parse_ident(p);
            int struct_idx = find_struct(p, name);
            if (struct_idx == -1) {
                fatal_error("parse_type: unknown struct, %.*s", name->len, name->data);
            }
            ty->def = p->structs + struct_idx;
        } else if (t->kind == TokenKind_keyword_union) {
            ty->kind = TypeKind_union;
            String* name = parse_ident(p);
            int union_idx = find_union(p, name);
            if (union_idx == -1) {
                fatal_error("parse_type: unknown union, %.*s", name->len, name->data);
            }
            ty->def = p->unions + union_idx;
        } else {
            unreachable();
        }
    }
    while (1) {
        if (peek_token(p)->kind == TokenKind_star) {
            next_token(p);
            ty = type_new_ptr(ty);
        } else {
            break;
        }
    }
    return ty;
}

AstNode* parse_prefix_expr(Parser* p) {
    TokenKind op = peek_token(p)->kind;
    if (op == TokenKind_minus) {
        next_token(p);
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_binary_expr(op, ast_new_int(0), operand);
    } else if (op == TokenKind_not) {
        next_token(p);
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_unary_expr(op, operand);
    } else if (op == TokenKind_and) {
        next_token(p);
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_ref_expr(operand);
    } else if (op == TokenKind_star) {
        next_token(p);
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_deref_expr(operand);
    } else if (op == TokenKind_plusplus) {
        next_token(p);
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_assign_add_expr(operand, ast_new_int(1));
    } else if (op == TokenKind_minusminus) {
        next_token(p);
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_assign_sub_expr(operand, ast_new_int(1));
    } else if (op == TokenKind_keyword_sizeof) {
        next_token(p);
        expect(p, TokenKind_paren_l);
        Token* next_tok = peek_token(p);
        Type* ty = NULL;
        if (next_tok->kind == TokenKind_ident) {
            int lvar_idx = find_lvar(p, &next_tok->raw);
            if (lvar_idx != -1) {
                next_token(p);
                ty = p->lvars[lvar_idx].ty;
            }
            int gvar_idx = find_gvar(p, &next_tok->raw);
            if (gvar_idx != -1) {
                next_token(p);
                ty = p->gvars[gvar_idx].ty;
            }
        }
        if (!ty) {
            ty = parse_type(p);
        }
        expect(p, TokenKind_paren_r);
        return ast_new_int(type_sizeof(ty));
    }
    return parse_postfix_expr(p);
}

AstNode* parse_multiplicative_expr(Parser* p) {
    AstNode* lhs = parse_prefix_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_star || op == TokenKind_slash || op == TokenKind_percent) {
            next_token(p);
            AstNode* rhs = parse_prefix_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

AstNode* parse_additive_expr(Parser* p) {
    AstNode* lhs = parse_multiplicative_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_plus) {
            next_token(p);
            AstNode* rhs = parse_multiplicative_expr(p);
            if (lhs->ty->base) {
                lhs = ast_new_binary_expr(
                    op, lhs, ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->base))));
            } else if (rhs->ty->base) {
                lhs = ast_new_binary_expr(
                    op, ast_new_binary_expr(TokenKind_star, lhs, ast_new_int(type_sizeof(rhs->ty->base))), rhs);
            } else {
                lhs = ast_new_binary_expr(op, lhs, rhs);
            }
        } else if (op == TokenKind_minus) {
            next_token(p);
            AstNode* rhs = parse_multiplicative_expr(p);
            if (lhs->ty->base) {
                if (rhs->ty->base) {
                    // (a - b) / sizeof(a)
                    lhs = ast_new_binary_expr(TokenKind_slash, ast_new_binary_expr(op, lhs, rhs),
                                              ast_new_int(type_sizeof(lhs->ty->base)));
                } else {
                    // a - b*sizeof(a)
                    lhs = ast_new_binary_expr(
                        op, lhs, ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->base))));
                }
            } else {
                lhs = ast_new_binary_expr(op, lhs, rhs);
            }
        } else {
            break;
        }
    }
    return lhs;
}

AstNode* parse_shift_expr(Parser* p) {
    AstNode* lhs = parse_additive_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_lshift || op == TokenKind_rshift) {
            next_token(p);
            AstNode* rhs = parse_additive_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

AstNode* parse_relational_expr(Parser* p) {
    AstNode* lhs = parse_shift_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_lt || op == TokenKind_le) {
            next_token(p);
            AstNode* rhs = parse_shift_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
        } else if (op == TokenKind_gt) {
            next_token(p);
            AstNode* rhs = parse_shift_expr(p);
            lhs = ast_new_binary_expr(TokenKind_lt, rhs, lhs);
        } else if (op == TokenKind_ge) {
            next_token(p);
            AstNode* rhs = parse_shift_expr(p);
            lhs = ast_new_binary_expr(TokenKind_le, rhs, lhs);
        } else {
            break;
        }
    }
    return lhs;
}

AstNode* parse_equality_expr(Parser* p) {
    AstNode* lhs = parse_relational_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_eq || op == TokenKind_ne) {
            next_token(p);
            AstNode* rhs = parse_relational_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

AstNode* parse_bitwise_or_expr(Parser* p) {
    AstNode* lhs = parse_equality_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_or) {
            next_token(p);
            AstNode* rhs = parse_equality_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
            lhs->ty = type_new(TypeKind_int);
        } else {
            break;
        }
    }
    return lhs;
}

AstNode* parse_logical_and_expr(Parser* p) {
    AstNode* lhs = parse_bitwise_or_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_andand) {
            next_token(p);
            AstNode* rhs = parse_bitwise_or_expr(p);
            AstNode* e = ast_new(AstNodeKind_logical_expr);
            e->node_op = op;
            e->node_lhs = lhs;
            e->node_rhs = rhs;
            e->ty = type_new(TypeKind_int);
            lhs = e;
        } else {
            break;
        }
    }
    return lhs;
}

AstNode* parse_logical_or_expr(Parser* p) {
    AstNode* lhs = parse_logical_and_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_oror) {
            next_token(p);
            AstNode* rhs = parse_logical_and_expr(p);
            AstNode* e = ast_new(AstNodeKind_logical_expr);
            e->node_op = op;
            e->node_lhs = lhs;
            e->node_rhs = rhs;
            e->ty = type_new(TypeKind_int);
            lhs = e;
        } else {
            break;
        }
    }
    return lhs;
}

AstNode* parse_assignment_expr(Parser* p) {
    AstNode* lhs = parse_logical_or_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_assign || op == TokenKind_assign_mul || op == TokenKind_assign_div ||
            op == TokenKind_assign_mod) {
            next_token(p);
            AstNode* rhs = parse_logical_or_expr(p);
            lhs = ast_new_assign_expr(op, lhs, rhs);
        } else if (op == TokenKind_assign_add) {
            next_token(p);
            AstNode* rhs = parse_logical_or_expr(p);
            lhs = ast_new_assign_add_expr(lhs, rhs);
        } else if (op == TokenKind_assign_sub) {
            next_token(p);
            AstNode* rhs = parse_logical_or_expr(p);
            lhs = ast_new_assign_sub_expr(lhs, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

AstNode* parse_comma_expr(Parser* p) {
    AstNode* lhs = parse_assignment_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_comma) {
            next_token(p);
            AstNode* rhs = parse_assignment_expr(p);
            AstNode* list = ast_new_list(2);
            ast_append(list, lhs);
            ast_append(list, rhs);
            list->ty = rhs->ty;
            lhs = list;
        } else {
            break;
        }
    }
    return lhs;
}

AstNode* parse_expr(Parser* p) {
    return parse_comma_expr(p);
}

AstNode* parse_return_stmt(Parser* p) {
    expect(p, TokenKind_keyword_return);
    if (peek_token(p)->kind == TokenKind_semicolon) {
        next_token(p);
        return ast_new(AstNodeKind_return_stmt);
    }

    AstNode* expr = parse_expr(p);
    expect(p, TokenKind_semicolon);

    AstNode* ret = ast_new(AstNodeKind_return_stmt);
    ret->node_expr = expr;
    return ret;
}

AstNode* parse_if_stmt(Parser* p) {
    expect(p, TokenKind_keyword_if);
    expect(p, TokenKind_paren_l);
    AstNode* cond = parse_expr(p);
    expect(p, TokenKind_paren_r);
    AstNode* then_body = parse_stmt(p);
    AstNode* else_body = NULL;
    if (peek_token(p)->kind == TokenKind_keyword_else) {
        next_token(p);
        else_body = parse_stmt(p);
    }

    AstNode* stmt = ast_new(AstNodeKind_if_stmt);
    stmt->node_cond = cond;
    stmt->node_then = then_body;
    stmt->node_else = else_body;
    return stmt;
}

AstNode* parse_var_decl(Parser* p) {
    Type* ty = parse_type(p);
    if (type_is_unsized(ty)) {
        fatal_error("parse_var_decl: invalid type for variable");
    }
    String* name = parse_ident(p);

    if (peek_token(p)->kind == TokenKind_bracket_l) {
        next_token(p);
        AstNode* size_expr = parse_expr(p);
        if (size_expr->kind != AstNodeKind_int_expr) {
            fatal_error("parse_var_decl: invalid array size");
        }
        int size = size_expr->node_int_value;
        expect(p, TokenKind_bracket_r);
        ty = type_new_array(ty, size);
    }

    AstNode* init = NULL;
    if (peek_token(p)->kind == TokenKind_assign) {
        next_token(p);
        init = parse_assignment_expr(p);
    }
    expect(p, TokenKind_semicolon);

    if (find_lvar_in_current_scope(p, name) != -1) {
        // TODO: use name's location.
        fatal_error("%s:%d: '%.*s' redeclared", peek_token(p)->loc.filename, peek_token(p)->loc.line, name->len,
                    name->data);
    }
    int stack_offset = add_lvar(p, name, ty, FALSE);

    AstNode* ret;
    if (init) {
        AstNode* lhs = ast_new(AstNodeKind_lvar);
        lhs->name = *name;
        lhs->node_stack_offset = stack_offset;
        lhs->ty = ty;
        AstNode* assign = ast_new_assign_expr(TokenKind_assign, lhs, init);
        ret = ast_new(AstNodeKind_expr_stmt);
        ret->node_expr = assign;
    } else {
        ret = ast_new(AstNodeKind_lvar_decl);
    }
    return ret;
}

AstNode* parse_for_stmt(Parser* p) {
    expect(p, TokenKind_keyword_for);
    expect(p, TokenKind_paren_l);
    AstNode* init = NULL;
    AstNode* cond = NULL;
    AstNode* update = NULL;
    enter_scope(p);
    if (peek_token(p)->kind != TokenKind_semicolon) {
        if (is_type_token(p, peek_token(p))) {
            init = parse_var_decl(p)->node_expr;
        } else {
            init = parse_expr(p);
            expect(p, TokenKind_semicolon);
        }
    } else {
        expect(p, TokenKind_semicolon);
    }
    if (peek_token(p)->kind != TokenKind_semicolon) {
        cond = parse_expr(p);
    } else {
        cond = ast_new_int(1);
    }
    expect(p, TokenKind_semicolon);
    if (peek_token(p)->kind != TokenKind_paren_r) {
        update = parse_expr(p);
    }
    expect(p, TokenKind_paren_r);
    AstNode* body = parse_stmt(p);
    leave_scope(p);

    AstNode* stmt = ast_new(AstNodeKind_for_stmt);
    stmt->node_cond = cond;
    stmt->node_init = init;
    stmt->node_update = update;
    stmt->node_body = body;
    return stmt;
}

AstNode* parse_while_stmt(Parser* p) {
    expect(p, TokenKind_keyword_while);
    expect(p, TokenKind_paren_l);
    AstNode* cond = parse_expr(p);
    expect(p, TokenKind_paren_r);
    AstNode* body = parse_stmt(p);

    AstNode* stmt = ast_new(AstNodeKind_for_stmt);
    stmt->node_cond = cond;
    stmt->node_body = body;
    return stmt;
}

AstNode* parse_do_while_stmt(Parser* p) {
    expect(p, TokenKind_keyword_do);
    AstNode* body = parse_stmt(p);
    expect(p, TokenKind_keyword_while);
    expect(p, TokenKind_paren_l);
    AstNode* cond = parse_expr(p);
    expect(p, TokenKind_paren_r);
    expect(p, TokenKind_semicolon);

    AstNode* stmt = ast_new(AstNodeKind_do_while_stmt);
    stmt->node_cond = cond;
    stmt->node_body = body;
    return stmt;
}

AstNode* parse_break_stmt(Parser* p) {
    expect(p, TokenKind_keyword_break);
    expect(p, TokenKind_semicolon);
    return ast_new(AstNodeKind_break_stmt);
}

AstNode* parse_continue_stmt(Parser* p) {
    expect(p, TokenKind_keyword_continue);
    expect(p, TokenKind_semicolon);
    return ast_new(AstNodeKind_continue_stmt);
}

AstNode* parse_expr_stmt(Parser* p) {
    AstNode* e = parse_expr(p);
    expect(p, TokenKind_semicolon);
    AstNode* stmt = ast_new(AstNodeKind_expr_stmt);
    stmt->node_expr = e;
    return stmt;
}

AstNode* parse_block_stmt(Parser* p) {
    AstNode* list = ast_new_list(4);
    expect(p, TokenKind_brace_l);
    enter_scope(p);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        AstNode* stmt = parse_stmt(p);
        ast_append(list, stmt);
    }
    leave_scope(p);
    expect(p, TokenKind_brace_r);
    return list;
}

AstNode* parse_empty_stmt(Parser* p) {
    next_token(p);
    return ast_new(AstNodeKind_nop);
}

AstNode* parse_stmt(Parser* p) {
    Token* t = peek_token(p);
    if (t->kind == TokenKind_keyword_return) {
        return parse_return_stmt(p);
    } else if (t->kind == TokenKind_keyword_if) {
        return parse_if_stmt(p);
    } else if (t->kind == TokenKind_keyword_for) {
        return parse_for_stmt(p);
    } else if (t->kind == TokenKind_keyword_while) {
        return parse_while_stmt(p);
    } else if (t->kind == TokenKind_keyword_do) {
        return parse_do_while_stmt(p);
    } else if (t->kind == TokenKind_keyword_break) {
        return parse_break_stmt(p);
    } else if (t->kind == TokenKind_keyword_continue) {
        return parse_continue_stmt(p);
    } else if (t->kind == TokenKind_brace_l) {
        return parse_block_stmt(p);
    } else if (t->kind == TokenKind_semicolon) {
        return parse_empty_stmt(p);
    } else if (is_type_token(p, t)) {
        return parse_var_decl(p);
    } else {
        return parse_expr_stmt(p);
    }
}

void register_params(Parser* p, AstNode* params) {
    for (int i = 0; i < params->node_len; ++i) {
        AstNode* param = params->node_items + i;
        add_lvar(p, &param->name, param->ty, TRUE);
    }
}

void register_func(Parser* p, const String* name, Type* ty) {
    p->funcs[p->n_funcs].name = *name;
    p->funcs[p->n_funcs].ty = ty;
    ++p->n_funcs;
}

AstNode* parse_param(Parser* p) {
    Type* ty = parse_type(p);
    String* name = NULL;
    TokenKind tk = peek_token(p)->kind;
    if (tk != TokenKind_comma && tk != TokenKind_paren_r) {
        name = parse_ident(p);
    }
    AstNode* param = ast_new(AstNodeKind_param);
    param->ty = ty;
    if (name) {
        param->name = *name;
    }
    return param;
}

AstNode* parse_param_list(Parser* p) {
    BOOL has_void = FALSE;
    AstNode* list = ast_new_list(6);
    while (peek_token(p)->kind != TokenKind_paren_r) {
        if (peek_token(p)->kind == TokenKind_ellipsis) {
            next_token(p);
            break;
        }
        AstNode* param = parse_param(p);
        has_void = has_void || param->ty->kind == TypeKind_void;
        ast_append(list, param);
        if (peek_token(p)->kind == TokenKind_comma) {
            next_token(p);
        } else {
            break;
        }
    }
    if (list->node_len > 6) {
        fatal_error("too many parameters");
    }
    if (has_void) {
        if (list->node_len != 1) {
            fatal_error("invalid use of void param");
        }
        list->node_len = 0;
    }
    return list;
}

AstNode* parse_global_var_decl(Parser* p, Type* ty, String* name) {
    if (type_is_unsized(ty)) {
        fatal_error("parse_global_var_decl: invalid type for variable");
    }

    if (peek_token(p)->kind == TokenKind_bracket_l) {
        next_token(p);
        AstNode* size_expr = parse_expr(p);
        if (size_expr->kind != AstNodeKind_int_expr) {
            fatal_error("parse_global_var_decl: invalid array size");
        }
        int size = size_expr->node_int_value;
        expect(p, TokenKind_bracket_r);
        ty = type_new_array(ty, size);
    }

    expect(p, TokenKind_semicolon);

    if (find_gvar(p, name) != -1) {
        fatal_error("parse_global_var_decl: %.*s redeclared", name->len, name->data);
    }

    p->gvars[p->n_gvars].name = *name;
    p->gvars[p->n_gvars].ty = ty;
    ++p->n_gvars;

    AstNode* ret = ast_new(AstNodeKind_gvar_decl);
    ret->name = *name;
    ret->ty = ty;
    return ret;
}

AstNode* parse_func_decl_or_def(Parser* p) {
    Type* ty = parse_type(p);
    String* name = parse_ident(p);

    if (peek_token(p)->kind != TokenKind_paren_l) {
        return parse_global_var_decl(p, ty, name);
    }

    expect(p, TokenKind_paren_l);
    register_func(p, name, ty);
    AstNode* params = parse_param_list(p);
    expect(p, TokenKind_paren_r);
    if (peek_token(p)->kind == TokenKind_semicolon) {
        next_token(p);
        return ast_new(AstNodeKind_func_decl);
    }
    enter_func(p);
    register_params(p, params);
    AstNode* body = parse_block_stmt(p);
    leave_func(p);
    AstNode* func = ast_new(AstNodeKind_func_def);
    func->ty = ty;
    func->name = *name;
    func->node_params = params;
    func->node_body = body;
    return func;
}

AstNode* parse_struct_member(Parser* p) {
    Type* ty = parse_type(p);
    String* name = parse_ident(p);
    expect(p, TokenKind_semicolon);
    AstNode* member = ast_new(AstNodeKind_struct_member);
    member->name = *name;
    member->ty = ty;
    return member;
}

AstNode* parse_struct_members(Parser* p) {
    AstNode* list = ast_new_list(4);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        AstNode* member = parse_struct_member(p);
        ast_append(list, member);
    }
    return list;
}

AstNode* parse_struct_decl_or_def(Parser* p) {
    expect(p, TokenKind_keyword_struct);
    String* name = parse_ident(p);

    if (peek_token(p)->kind != TokenKind_semicolon && peek_token(p)->kind != TokenKind_brace_l) {
        p->pos = p->pos - 2;
        return parse_func_decl_or_def(p);
    }

    int struct_idx = find_struct(p, name);
    if (struct_idx == -1) {
        struct_idx = p->n_structs;
        p->structs[struct_idx].kind = AstNodeKind_struct_def;
        p->structs[struct_idx].name = *name;
        ++p->n_structs;
    }
    if (peek_token(p)->kind == TokenKind_semicolon) {
        next_token(p);
        return ast_new(AstNodeKind_struct_decl);
    }
    if (p->structs[struct_idx].node_members) {
        fatal_error("parse_struct_decl_or_def: struct %.*s redefined", name->len, name->data);
    }
    expect(p, TokenKind_brace_l);
    AstNode* members = parse_struct_members(p);
    expect(p, TokenKind_brace_r);
    expect(p, TokenKind_semicolon);
    p->structs[struct_idx].node_members = members;
    return p->structs + struct_idx;
}

AstNode* parse_union_member(Parser* p) {
    Type* ty = parse_type(p);
    String* name = parse_ident(p);
    expect(p, TokenKind_semicolon);
    AstNode* member = ast_new(AstNodeKind_union_member);
    member->name = *name;
    member->ty = ty;
    return member;
}

AstNode* parse_union_members(Parser* p) {
    AstNode* list = ast_new_list(4);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        AstNode* member = parse_union_member(p);
        ast_append(list, member);
    }
    return list;
}

AstNode* parse_union_decl_or_def(Parser* p) {
    expect(p, TokenKind_keyword_union);
    String* name = parse_ident(p);

    if (peek_token(p)->kind != TokenKind_semicolon && peek_token(p)->kind != TokenKind_brace_l) {
        p->pos = p->pos - 2;
        return parse_func_decl_or_def(p);
    }

    int union_idx = find_union(p, name);
    if (union_idx == -1) {
        union_idx = p->n_unions;
        p->unions[union_idx].kind = AstNodeKind_union_def;
        p->unions[union_idx].name = *name;
        ++p->n_unions;
    }
    if (peek_token(p)->kind == TokenKind_semicolon) {
        next_token(p);
        return ast_new(AstNodeKind_union_decl);
    }
    if (p->unions[union_idx].node_members) {
        fatal_error("parse_union_decl_or_def: union %.*s redefined", name->len, name->data);
    }
    expect(p, TokenKind_brace_l);
    AstNode* members = parse_union_members(p);
    expect(p, TokenKind_brace_r);
    expect(p, TokenKind_semicolon);
    p->unions[union_idx].node_members = members;
    return p->unions + union_idx;
}

AstNode* parse_enum_member(Parser* p) {
    String* name = parse_ident(p);
    AstNode* member = ast_new(AstNodeKind_enum_member);
    member->name = *name;
    return member;
}

AstNode* parse_enum_members(Parser* p) {
    int next_value = 0;
    AstNode* list = ast_new_list(16);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        AstNode* member = parse_enum_member(p);
        member->node_int_value = next_value;
        ++next_value;
        ast_append(list, member);
        if (peek_token(p)->kind != TokenKind_comma) {
            break;
        }
        next_token(p);
    }
    return list;
}

AstNode* parse_enum_def(Parser* p) {
    expect(p, TokenKind_keyword_enum);
    String* name = parse_ident(p);

    if (peek_token(p)->kind != TokenKind_brace_l) {
        p->pos = p->pos - 2;
        return parse_func_decl_or_def(p);
    }

    int enum_idx = find_enum(p, name);
    if (enum_idx == -1) {
        enum_idx = p->n_enums;
        p->enums[enum_idx].kind = AstNodeKind_enum_def;
        p->enums[enum_idx].name = *name;
        ++p->n_enums;
    } else {
        fatal_error("parse_enum_def: enum %.*s redefined", name->len, name->data);
    }
    expect(p, TokenKind_brace_l);
    AstNode* members = parse_enum_members(p);
    expect(p, TokenKind_brace_r);
    expect(p, TokenKind_semicolon);
    p->enums[enum_idx].node_members = members;
    return p->enums + enum_idx;
}

AstNode* parse_typedef_decl(Parser* p) {
    expect(p, TokenKind_keyword_typedef);
    Type* ty = parse_type(p);
    String* name = parse_ident(p);
    expect(p, TokenKind_semicolon);
    AstNode* decl = ast_new(AstNodeKind_typedef_decl);
    decl->name = *name;
    decl->ty = ty;
    p->typedefs[p->n_typedefs].name = *name;
    p->typedefs[p->n_typedefs].ty = ty;
    ++p->n_typedefs;
    return decl;
}

AstNode* parse_extern_var_decl(Parser* p) {
    expect(p, TokenKind_keyword_extern);
    Type* ty = parse_type(p);
    if (type_is_unsized(ty)) {
        fatal_error("parse_extern_var_decl: invalid type for variable");
    }
    String* name = parse_ident(p);
    expect(p, TokenKind_semicolon);

    if (find_lvar(p, name) != -1 || find_gvar(p, name) != -1) {
        fatal_error("parse_extern_var_decl: %.*s redeclared", name->len, name->data);
    }
    p->gvars[p->n_gvars].name = *name;
    p->gvars[p->n_gvars].ty = ty;
    ++p->n_gvars;

    return ast_new(AstNodeKind_gvar_decl);
}

AstNode* parse_toplevel(Parser* p) {
    TokenKind tk = peek_token(p)->kind;
    if (tk == TokenKind_keyword_struct) {
        return parse_struct_decl_or_def(p);
    } else if (tk == TokenKind_keyword_union) {
        return parse_union_decl_or_def(p);
    } else if (tk == TokenKind_keyword_enum) {
        return parse_enum_def(p);
    } else if (tk == TokenKind_keyword_typedef) {
        return parse_typedef_decl(p);
    } else if (tk == TokenKind_keyword_extern) {
        return parse_extern_var_decl(p);
    } else {
        return parse_func_decl_or_def(p);
    }
}

Program* parse(TokenArray* tokens) {
    Parser* p = parser_new(tokens);
    AstNode* funcs = ast_new_list(32);
    AstNode* vars = ast_new_list(16);
    while (eof(p)) {
        AstNode* n = parse_toplevel(p);
        if (n->kind == AstNodeKind_func_def) {
            ast_append(funcs, n);
        } else if (n->kind == AstNodeKind_gvar_decl && n->ty) {
            ast_append(vars, n);
        }
    }
    Program* prog = calloc(1, sizeof(Program));
    prog->funcs = funcs;
    prog->vars = vars;
    prog->str_literals = p->str_literals;
    return prog;
}
