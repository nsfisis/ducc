#include "parse.h"
#include "common.h"
#include "tokenize.h"

struct LocalVar {
    const char* name;
    Type* ty;
    int stack_offset;
};
typedef struct LocalVar LocalVar;

struct LocalVarArray {
    size_t len;
    size_t capacity;
    LocalVar* data;
};
typedef struct LocalVarArray LocalVarArray;

static void lvars_init(LocalVarArray* lvars) {
    lvars->len = 0;
    lvars->capacity = 4;
    lvars->data = calloc(lvars->capacity, sizeof(LocalVar));
}

static void lvars_reserve(LocalVarArray* lvars, size_t size) {
    if (size <= lvars->capacity)
        return;
    while (lvars->capacity < size) {
        lvars->capacity *= 2;
    }
    lvars->data = realloc(lvars->data, lvars->capacity * sizeof(LocalVar));
    memset(lvars->data + lvars->len, 0, (lvars->capacity - lvars->len) * sizeof(LocalVar));
}

static LocalVar* lvars_push_new(LocalVarArray* lvars) {
    lvars_reserve(lvars, lvars->len + 1);
    return &lvars->data[lvars->len++];
}

struct ScopedSymbol {
    const char* name;
    int index;
};
typedef struct ScopedSymbol ScopedSymbol;

struct ScopedSymbolArray {
    size_t len;
    size_t capacity;
    ScopedSymbol* data;
};
typedef struct ScopedSymbolArray ScopedSymbolArray;

static void scopedsymbols_init(ScopedSymbolArray* syms) {
    syms->len = 0;
    syms->capacity = 4;
    syms->data = calloc(syms->capacity, sizeof(ScopedSymbol));
}

static void scopedsymbols_reserve(ScopedSymbolArray* syms, size_t size) {
    if (size <= syms->capacity)
        return;
    while (syms->capacity < size) {
        syms->capacity *= 2;
    }
    syms->data = realloc(syms->data, syms->capacity * sizeof(ScopedSymbol));
    memset(syms->data + syms->len, 0, (syms->capacity - syms->len) * sizeof(ScopedSymbol));
}

static ScopedSymbol* scopedsymbols_push_new(ScopedSymbolArray* syms) {
    scopedsymbols_reserve(syms, syms->len + 1);
    return &syms->data[syms->len++];
}

struct Scope {
    struct Scope* outer;
    ScopedSymbolArray syms;
};
typedef struct Scope Scope;

struct GlobalVar {
    const char* name;
    Type* ty;
};
typedef struct GlobalVar GlobalVar;

struct GlobalVarArray {
    size_t len;
    size_t capacity;
    GlobalVar* data;
};
typedef struct GlobalVarArray GlobalVarArray;

static void gvars_init(GlobalVarArray* gvars) {
    gvars->len = 0;
    gvars->capacity = 4;
    gvars->data = calloc(gvars->capacity, sizeof(GlobalVar));
}

static void gvars_reserve(GlobalVarArray* gvars, size_t size) {
    if (size <= gvars->capacity)
        return;
    while (gvars->capacity < size) {
        gvars->capacity *= 2;
    }
    gvars->data = realloc(gvars->data, gvars->capacity * sizeof(GlobalVar));
    memset(gvars->data + gvars->len, 0, (gvars->capacity - gvars->len) * sizeof(GlobalVar));
}

static GlobalVar* gvars_push_new(GlobalVarArray* gvars) {
    gvars_reserve(gvars, gvars->len + 1);
    return &gvars->data[gvars->len++];
}

struct Func {
    const char* name;
    Type* ty;
};
typedef struct Func Func;

struct FuncArray {
    size_t len;
    size_t capacity;
    Func* data;
};
typedef struct FuncArray FuncArray;

static void funcs_init(FuncArray* funcs) {
    funcs->len = 0;
    funcs->capacity = 32;
    funcs->data = calloc(funcs->capacity, sizeof(Func));
}

static void funcs_reserve(FuncArray* funcs, size_t size) {
    if (size <= funcs->capacity)
        return;
    while (funcs->capacity < size) {
        funcs->capacity *= 2;
    }
    funcs->data = realloc(funcs->data, funcs->capacity * sizeof(Func));
    memset(funcs->data + funcs->len, 0, (funcs->capacity - funcs->len) * sizeof(Func));
}

static Func* funcs_push_new(FuncArray* funcs) {
    funcs_reserve(funcs, funcs->len + 1);
    return &funcs->data[funcs->len++];
}

struct Parser {
    TokenArray* tokens;
    int pos;
    LocalVarArray lvars;
    Scope* scope;
    GlobalVarArray gvars;
    FuncArray funcs;
    AstNode* structs;
    AstNode* unions;
    AstNode* enums;
    AstNode* typedefs;
    StrArray str_literals;
};
typedef struct Parser Parser;

static Parser* parser_new(TokenArray* tokens) {
    Parser* p = calloc(1, sizeof(Parser));
    p->tokens = tokens;
    gvars_init(&p->gvars);
    funcs_init(&p->funcs);
    p->structs = ast_new_list(4);
    p->unions = ast_new_list(4);
    p->enums = ast_new_list(4);
    p->typedefs = ast_new_list(16);
    strings_init(&p->str_literals);

    Func* va_start_func = funcs_push_new(&p->funcs);
    va_start_func->name = "__ducc_va_start";
    va_start_func->ty = calloc(1, sizeof(Type));
    va_start_func->ty->kind = TypeKind_void;

    return p;
}

static Token* peek_token(Parser* p) {
    return &p->tokens->data[p->pos];
}

static Token* peek_token2(Parser* p) {
    return &p->tokens->data[p->pos + 1];
}

static Token* next_token(Parser* p) {
    return &p->tokens->data[p->pos++];
}

static Token* consume_token_if(Parser* p, TokenKind expected) {
    if (peek_token(p)->kind == expected) {
        return next_token(p);
    } else {
        return NULL;
    }
}

static BOOL eof(Parser* p) {
    return peek_token(p)->kind != TokenKind_eof;
}

static Token* expect(Parser* p, TokenKind expected) {
    Token* t = next_token(p);
    if (t->kind == expected) {
        return t;
    }
    fatal_error("%s:%d: expected '%s', but got '%s'", t->loc.filename, t->loc.line, token_kind_stringify(expected),
                token_stringify(t));
}

static int find_lvar_in_scope(Parser* p, Scope* scope, const char* name) {
    for (size_t i = 0; i < scope->syms.len; ++i) {
        ScopedSymbol* sym = &scope->syms.data[i];
        if (sym->name && strcmp(sym->name, name) == 0) {
            return sym->index;
        }
    }
    return -1;
}

static int find_lvar_in_current_scope(Parser* p, const char* name) {
    return find_lvar_in_scope(p, p->scope, name);
}

static int find_lvar(Parser* p, const char* name) {
    Scope* scope = p->scope;
    while (scope) {
        int idx = find_lvar_in_scope(p, scope, name);
        if (idx != -1)
            return idx;
        scope = scope->outer;
    }
    return -1;
}

static int calc_stack_offset(Parser* p, Type* ty, BOOL is_param) {
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
    if (p->lvars.len == 0) {
        offset = 0;
    } else {
        offset = p->lvars.data[p->lvars.len - 1].stack_offset;
    }

    offset += type_sizeof(ty);
    return to_aligned(offset, align);
}

static int add_lvar(Parser* p, const char* name, Type* ty, BOOL is_param) {
    int stack_offset = calc_stack_offset(p, ty, is_param);
    LocalVar* lvar = lvars_push_new(&p->lvars);
    lvar->name = name;
    lvar->ty = ty;
    lvar->stack_offset = stack_offset;
    ScopedSymbol* sym = scopedsymbols_push_new(&p->scope->syms);
    sym->name = name;
    sym->index = p->lvars.len - 1;
    return stack_offset;
}

static AstNode* generate_temporary_lvar(Parser* p, Type* ty) {
    int stack_offset = add_lvar(p, NULL, ty, FALSE);
    AstNode* lvar = ast_new(AstNodeKind_lvar);
    lvar->name = NULL;
    lvar->node_stack_offset = stack_offset;
    lvar->ty = ty;
    return lvar;
}

static int find_gvar(Parser* p, const char* name) {
    for (size_t i = 0; i < p->gvars.len; ++i) {
        if (strcmp(p->gvars.data[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int find_func(Parser* p, const char* name) {
    for (size_t i = 0; i < p->funcs.len; ++i) {
        if (strcmp(p->funcs.data[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int find_struct(Parser* p, const char* name) {
    for (int i = 0; i < p->structs->node_len; ++i) {
        if (strcmp(p->structs->node_items[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int find_union(Parser* p, const char* name) {
    for (int i = 0; i < p->unions->node_len; ++i) {
        if (strcmp(p->unions->node_items[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int find_enum(Parser* p, const char* name) {
    for (int i = 0; i < p->enums->node_len; ++i) {
        if (strcmp(p->enums->node_items[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int find_enum_member(Parser* p, const char* name) {
    for (int i = 0; i < p->enums->node_len; ++i) {
        for (int j = 0; j < p->enums->node_items[i].node_members->node_len; ++j) {
            if (strcmp(p->enums->node_items[i].node_members->node_items[j].name, name) == 0) {
                return i * 1000 + j;
            }
        }
    }
    return -1;
}

static int find_typedef(Parser* p, const char* name) {
    for (int i = 0; i < p->typedefs->node_len; ++i) {
        if (strcmp(p->typedefs->node_items[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static void enter_scope(Parser* p) {
    Scope* outer_scope = p->scope;
    p->scope = calloc(1, sizeof(Scope));
    p->scope->outer = outer_scope;
    scopedsymbols_init(&p->scope->syms);
}

static void leave_scope(Parser* p) {
    p->scope = p->scope->outer;
}

static void enter_func(Parser* p) {
    lvars_init(&p->lvars);
    enter_scope(p);
}

static void leave_func(Parser* p) {
    leave_scope(p);
}

static AstNode* parse_assignment_expr(Parser* p);
static AstNode* parse_expr(Parser* p);
static AstNode* parse_stmt(Parser* p);

static Token* parse_ident(Parser* p) {
    return expect(p, TokenKind_ident);
}

static int register_str_literal(Parser* p, const char* s) {
    return strings_push(&p->str_literals, s);
}

static AstNode* parse_primary_expr(Parser* p) {
    Token* t = next_token(p);
    if (t->kind == TokenKind_literal_int) {
        return ast_new_int(t->value.integer);
    } else if (t->kind == TokenKind_literal_str) {
        AstNode* e = ast_new(AstNodeKind_str_expr);
        e->node_idx = register_str_literal(p, t->value.string);
        e->ty = type_new_static_string(strlen(t->value.string));
        return e;
    } else if (t->kind == TokenKind_paren_l) {
        AstNode* e = parse_expr(p);
        expect(p, TokenKind_paren_r);
        return e;
    } else if (t->kind == TokenKind_ident) {
        const char* name = t->value.string;

        if (peek_token(p)->kind == TokenKind_paren_l) {
            AstNode* e = ast_new(AstNodeKind_func_call);
            int func_idx = find_func(p, name);
            if (func_idx == -1) {
                fatal_error("undefined function: %s", name);
            }
            e->name = name;
            e->ty = p->funcs.data[func_idx].ty;
            return e;
        }

        int lvar_idx = find_lvar(p, name);
        if (lvar_idx == -1) {
            int gvar_idx = find_gvar(p, name);
            if (gvar_idx == -1) {
                int enum_member_idx = find_enum_member(p, name);
                if (enum_member_idx == -1) {
                    fatal_error("undefined variable: %s", name);
                }
                int enum_idx = enum_member_idx / 1000;
                int n = enum_member_idx % 1000;
                AstNode* e = ast_new_int(p->enums->node_items[enum_idx].node_members->node_items[n].node_int_value);
                e->ty = type_new(TypeKind_enum);
                e->ty->ref.defs = p->enums;
                e->ty->ref.index = enum_idx;
                return e;
            }
            AstNode* e = ast_new(AstNodeKind_gvar);
            e->name = name;
            e->ty = p->gvars.data[gvar_idx].ty;
            return e;
        }

        AstNode* e = ast_new(AstNodeKind_lvar);
        e->name = name;
        e->node_stack_offset = p->lvars.data[lvar_idx].stack_offset;
        e->ty = p->lvars.data[lvar_idx].ty;
        return e;
    } else {
        fatal_error("%s:%d: expected an expression, but got '%s'", t->loc.filename, t->loc.line, token_stringify(t));
    }
}

static AstNode* parse_arg_list(Parser* p) {
    AstNode* list = ast_new_list(6);
    while (peek_token(p)->kind != TokenKind_paren_r) {
        AstNode* arg = parse_assignment_expr(p);
        ast_append(list, arg);
        if (!consume_token_if(p, TokenKind_comma)) {
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
static AstNode* create_new_postfix_inc_or_dec(Parser* p, AstNode* e, TokenKind op) {
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

static AstNode* parse_postfix_expr(Parser* p) {
    AstNode* ret = parse_primary_expr(p);
    while (1) {
        if (consume_token_if(p, TokenKind_paren_l)) {
            AstNode* args = parse_arg_list(p);
            expect(p, TokenKind_paren_r);
            ret->node_args = args;
        } else if (consume_token_if(p, TokenKind_bracket_l)) {
            AstNode* idx = parse_expr(p);
            expect(p, TokenKind_bracket_r);
            idx = ast_new_binary_expr(TokenKind_star, idx, ast_new_int(type_sizeof(ret->ty->base)));
            ret = ast_new_deref_expr(ast_new_binary_expr(TokenKind_plus, ret, idx));
        } else if (consume_token_if(p, TokenKind_dot)) {
            const Token* name = parse_ident(p);
            ret = ast_new_member_access_expr(ast_new_ref_expr(ret), name->value.string);
        } else if (consume_token_if(p, TokenKind_arrow)) {
            const Token* name = parse_ident(p);
            ret = ast_new_member_access_expr(ret, name->value.string);
        } else if (consume_token_if(p, TokenKind_plusplus)) {
            ret = create_new_postfix_inc_or_dec(p, ret, TokenKind_plusplus);
        } else if (consume_token_if(p, TokenKind_minusminus)) {
            ret = create_new_postfix_inc_or_dec(p, ret, TokenKind_minusminus);
        } else {
            break;
        }
    }
    return ret;
}

static BOOL is_typedef_name(Parser* p, Token* tok) {
    return tok->kind == TokenKind_ident && find_typedef(p, tok->value.string) != -1;
}

static BOOL is_type_token(Parser* p, Token* token) {
    if (token->kind == TokenKind_keyword_int || token->kind == TokenKind_keyword_short ||
        token->kind == TokenKind_keyword_long || token->kind == TokenKind_keyword_char ||
        token->kind == TokenKind_keyword_void || token->kind == TokenKind_keyword_enum ||
        token->kind == TokenKind_keyword_struct || token->kind == TokenKind_keyword_union ||
        token->kind == TokenKind_keyword_const || token->kind == TokenKind_keyword_static) {
        return TRUE;
    }
    if (token->kind != TokenKind_ident) {
        return FALSE;
    }
    return find_typedef(p, token->value.string) != -1;
}

static Type* parse_type_name(Parser* p);

static AstNode* parse_prefix_expr(Parser* p) {
    TokenKind op = peek_token(p)->kind;
    if (consume_token_if(p, TokenKind_minus)) {
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_binary_expr(op, ast_new_int(0), operand);
    } else if (consume_token_if(p, TokenKind_not)) {
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_unary_expr(op, operand);
    } else if (consume_token_if(p, TokenKind_and)) {
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_ref_expr(operand);
    } else if (consume_token_if(p, TokenKind_star)) {
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_deref_expr(operand);
    } else if (consume_token_if(p, TokenKind_plusplus)) {
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_assign_add_expr(operand, ast_new_int(1));
    } else if (consume_token_if(p, TokenKind_minusminus)) {
        AstNode* operand = parse_prefix_expr(p);
        return ast_new_assign_sub_expr(operand, ast_new_int(1));
    } else if (consume_token_if(p, TokenKind_keyword_sizeof)) {
        expect(p, TokenKind_paren_l);
        Token* next_tok = peek_token(p);
        Type* ty = NULL;
        if (next_tok->kind == TokenKind_ident) {
            int lvar_idx = find_lvar(p, next_tok->value.string);
            if (lvar_idx != -1) {
                next_token(p);
                ty = p->lvars.data[lvar_idx].ty;
            }
            int gvar_idx = find_gvar(p, next_tok->value.string);
            if (gvar_idx != -1) {
                next_token(p);
                ty = p->gvars.data[gvar_idx].ty;
            }
        }
        if (!ty) {
            ty = parse_type_name(p);
        }
        expect(p, TokenKind_paren_r);
        return ast_new_int(type_sizeof(ty));
    }
    return parse_postfix_expr(p);
}

static AstNode* parse_cast_expr(Parser* p) {
    if (peek_token(p)->kind == TokenKind_paren_l && is_type_token(p, peek_token2(p))) {
        next_token(p);
        Type* ty = parse_type_name(p);
        expect(p, TokenKind_paren_r);

        // TODO: check whether the original type can be casted to the result type.
        AstNode* e = parse_cast_expr(p);
        e->ty = ty;
        return e;
    }
    return parse_prefix_expr(p);
}

static AstNode* parse_multiplicative_expr(Parser* p) {
    AstNode* lhs = parse_cast_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_star || op == TokenKind_slash || op == TokenKind_percent) {
            next_token(p);
            AstNode* rhs = parse_cast_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

static AstNode* parse_additive_expr(Parser* p) {
    AstNode* lhs = parse_multiplicative_expr(p);
    while (1) {
        if (consume_token_if(p, TokenKind_plus)) {
            AstNode* rhs = parse_multiplicative_expr(p);
            if (lhs->ty->base) {
                lhs = ast_new_binary_expr(
                    TokenKind_plus, lhs,
                    ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->base))));
            } else if (rhs->ty->base) {
                lhs = ast_new_binary_expr(
                    TokenKind_plus, ast_new_binary_expr(TokenKind_star, lhs, ast_new_int(type_sizeof(rhs->ty->base))),
                    rhs);
            } else {
                lhs = ast_new_binary_expr(TokenKind_plus, lhs, rhs);
            }
        } else if (consume_token_if(p, TokenKind_minus)) {
            AstNode* rhs = parse_multiplicative_expr(p);
            if (lhs->ty->base) {
                if (rhs->ty->base) {
                    // (a - b) / sizeof(a)
                    lhs = ast_new_binary_expr(TokenKind_slash, ast_new_binary_expr(TokenKind_minus, lhs, rhs),
                                              ast_new_int(type_sizeof(lhs->ty->base)));
                } else {
                    // a - b*sizeof(a)
                    lhs = ast_new_binary_expr(
                        TokenKind_minus, lhs,
                        ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->base))));
                }
            } else {
                lhs = ast_new_binary_expr(TokenKind_minus, lhs, rhs);
            }
        } else {
            break;
        }
    }
    return lhs;
}

static AstNode* parse_shift_expr(Parser* p) {
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

static AstNode* parse_relational_expr(Parser* p) {
    AstNode* lhs = parse_shift_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_lt || op == TokenKind_le) {
            next_token(p);
            AstNode* rhs = parse_shift_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
        } else if (consume_token_if(p, TokenKind_gt)) {
            AstNode* rhs = parse_shift_expr(p);
            lhs = ast_new_binary_expr(TokenKind_lt, rhs, lhs);
        } else if (consume_token_if(p, TokenKind_ge)) {
            AstNode* rhs = parse_shift_expr(p);
            lhs = ast_new_binary_expr(TokenKind_le, rhs, lhs);
        } else {
            break;
        }
    }
    return lhs;
}

static AstNode* parse_equality_expr(Parser* p) {
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

static AstNode* parse_bitwise_or_expr(Parser* p) {
    AstNode* lhs = parse_equality_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (consume_token_if(p, TokenKind_or)) {
            AstNode* rhs = parse_equality_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
            lhs->ty = type_new(TypeKind_int);
        } else {
            break;
        }
    }
    return lhs;
}

static AstNode* parse_logical_and_expr(Parser* p) {
    AstNode* lhs = parse_bitwise_or_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (consume_token_if(p, TokenKind_andand)) {
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

static AstNode* parse_logical_or_expr(Parser* p) {
    AstNode* lhs = parse_logical_and_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (consume_token_if(p, TokenKind_oror)) {
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

static AstNode* parse_conditional_expr(Parser* p) {
    AstNode* e = parse_logical_or_expr(p);
    if (consume_token_if(p, TokenKind_question)) {
        AstNode* then_expr = parse_expr(p);
        expect(p, TokenKind_colon);
        AstNode* else_expr = parse_assignment_expr(p);
        AstNode* ret = ast_new(AstNodeKind_cond_expr);
        ret->node_cond = e;
        ret->node_then = then_expr;
        ret->node_else = else_expr;
        ret->ty = then_expr->ty;
        return ret;
    } else {
        return e;
    }
}

// constant-expression:
//     conditional-expression
static AstNode* parse_constant_expression(Parser* p) {
    return parse_conditional_expr(p);
}

static AstNode* parse_assignment_expr(Parser* p) {
    AstNode* lhs = parse_conditional_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        // TODO: check if the lhs is unary expression.
        if (op == TokenKind_assign || op == TokenKind_assign_mul || op == TokenKind_assign_div ||
            op == TokenKind_assign_mod) {
            next_token(p);
            AstNode* rhs = parse_assignment_expr(p);
            lhs = ast_new_assign_expr(op, lhs, rhs);
        } else if (consume_token_if(p, TokenKind_assign_add)) {
            AstNode* rhs = parse_assignment_expr(p);
            lhs = ast_new_assign_add_expr(lhs, rhs);
        } else if (consume_token_if(p, TokenKind_assign_sub)) {
            AstNode* rhs = parse_assignment_expr(p);
            lhs = ast_new_assign_sub_expr(lhs, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

static AstNode* parse_comma_expr(Parser* p) {
    AstNode* lhs = parse_assignment_expr(p);
    while (1) {
        if (consume_token_if(p, TokenKind_comma)) {
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

static AstNode* parse_expr(Parser* p) {
    return parse_comma_expr(p);
}

static AstNode* parse_return_stmt(Parser* p) {
    expect(p, TokenKind_keyword_return);
    if (consume_token_if(p, TokenKind_semicolon)) {
        return ast_new(AstNodeKind_return_stmt);
    }

    AstNode* expr = parse_expr(p);
    expect(p, TokenKind_semicolon);

    AstNode* ret = ast_new(AstNodeKind_return_stmt);
    ret->node_expr = expr;
    return ret;
}

static AstNode* parse_if_stmt(Parser* p) {
    expect(p, TokenKind_keyword_if);
    expect(p, TokenKind_paren_l);
    AstNode* cond = parse_expr(p);
    expect(p, TokenKind_paren_r);
    AstNode* then_body = parse_stmt(p);
    AstNode* else_body = NULL;
    if (consume_token_if(p, TokenKind_keyword_else)) {
        else_body = parse_stmt(p);
    }

    AstNode* stmt = ast_new(AstNodeKind_if_stmt);
    stmt->node_cond = cond;
    stmt->node_then = then_body;
    stmt->node_else = else_body;
    return stmt;
}

// pointer:
//     '*' TODO attribute-specifier-sequence? TODO type-qualifier-list?
//     '*' TODO attribute-specifier-sequence? TODO type-qualifier-list? pointer
static Type* parse_pointer_opt(Parser* p, Type* ty) {
    while (peek_token(p)->kind == TokenKind_star) {
        next_token(p);
        ty = type_new_ptr(ty);
    }
    return ty;
}

// array-declarator:
//     direct-declarator '[' TODO type-qualifier-list? TODO assignment-expression? ']'
//     TODO direct-declarator '[' 'static' type-qualifier-list? assignment-expression? ']'
//     TODO direct-declarator '[' type-qualifier-list 'static' assignment-expression? ']'
//     TODO direct-declarator '[' type-qualifier-list? '*' ']'
static Type* parse_array_declarator_suffix(Parser* p, Type* ty) {
    next_token(p); // skip '['

    AstNode* size_expr = parse_expr(p);
    if (size_expr->kind != AstNodeKind_int_expr) {
        fatal_error("parse_var_decl: invalid array size");
    }
    int size = size_expr->node_int_value;
    expect(p, TokenKind_bracket_r);

    return type_new_array(ty, size);
}

// direct-declarator:
//     identifier TODO attribute-specifier-sequence?
//     TODO '(' declarator ')'
//     array-declarator TODO attribute-specifier-sequence?
//     TODO function-declarator TODO attribute-specifier-sequence?
static AstNode* parse_direct_declarator(Parser* p, Type* ty) {
    const Token* name = parse_ident(p);
    while (1) {
        if (peek_token(p)->kind == TokenKind_bracket_l) {
            ty = parse_array_declarator_suffix(p, ty);
        } else {
            break;
        }
    }

    AstNode* ret = ast_new(AstNodeKind_declarator);
    ret->name = name->value.string;
    ret->ty = ty;
    return ret;
}

// declarator:
//     pointer? direct-declarator
static AstNode* parse_declarator(Parser* p, Type* ty) {
    ty = parse_pointer_opt(p, ty);
    return parse_direct_declarator(p, ty);
}

// initializer:
//     assignment-expression
//     TODO braced-initializer
static AstNode* parse_initializer(Parser* p) {
    return parse_assignment_expr(p);
}

// init-declarator:
//     declarator
//     declarator '=' initializer
static AstNode* parse_init_declarator(Parser* p, Type* ty) {
    AstNode* decl = parse_declarator(p, ty);
    AstNode* init = NULL;
    if (consume_token_if(p, TokenKind_assign)) {
        init = parse_initializer(p);
    }
    decl->node_init = init;
    return decl;
}

// init-declarator-list:
//     init-declarator
//     init-declarator-list ',' init-declarator
static AstNode* parse_init_declarator_list(Parser* p, Type* ty) {
    AstNode* list = ast_new_list(1);
    while (1) {
        AstNode* d = parse_init_declarator(p, ty);
        ast_append(list, d);
        if (!consume_token_if(p, TokenKind_comma)) {
            break;
        }
    }
    return list;
}

static AstNode* parse_var_decl(Parser* p) {
    Type* base_ty = parse_type_name(p);
    if (type_is_unsized(base_ty)) {
        fatal_error("parse_var_decl: invalid type for variable");
    }

    AstNode* decls = parse_init_declarator_list(p, base_ty);
    expect(p, TokenKind_semicolon);

    for (int i = 0; i < decls->node_len; ++i) {
        AstNode* decl = &decls->node_items[i];

        if (find_lvar_in_current_scope(p, decl->name) != -1) {
            // TODO: use name's location.
            fatal_error("%s:%d: '%s' redeclared", peek_token(p)->loc.filename, peek_token(p)->loc.line, decl->name);
        }
        int stack_offset = add_lvar(p, decl->name, decl->ty, FALSE);

        if (decl->node_init) {
            AstNode* lhs = ast_new(AstNodeKind_lvar);
            lhs->name = decl->name;
            lhs->node_stack_offset = stack_offset;
            lhs->ty = decl->ty;
            AstNode* assign = ast_new_assign_expr(TokenKind_assign, lhs, decl->node_init);
            decl->kind = AstNodeKind_expr_stmt;
            decl->node_expr = assign;
        } else {
            decl->kind = AstNodeKind_nop;
        }
    }
    return decls;
}

static AstNode* parse_for_stmt(Parser* p) {
    expect(p, TokenKind_keyword_for);
    expect(p, TokenKind_paren_l);
    AstNode* init = NULL;
    AstNode* cond = NULL;
    AstNode* update = NULL;
    enter_scope(p);
    if (peek_token(p)->kind != TokenKind_semicolon) {
        if (is_type_token(p, peek_token(p))) {
            init = parse_var_decl(p)->node_items[0].node_expr;
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

static AstNode* parse_while_stmt(Parser* p) {
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

static AstNode* parse_do_while_stmt(Parser* p) {
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

static AstNode* parse_break_stmt(Parser* p) {
    expect(p, TokenKind_keyword_break);
    expect(p, TokenKind_semicolon);
    return ast_new(AstNodeKind_break_stmt);
}

static AstNode* parse_continue_stmt(Parser* p) {
    expect(p, TokenKind_keyword_continue);
    expect(p, TokenKind_semicolon);
    return ast_new(AstNodeKind_continue_stmt);
}

static AstNode* parse_expr_stmt(Parser* p) {
    AstNode* e = parse_expr(p);
    expect(p, TokenKind_semicolon);
    AstNode* stmt = ast_new(AstNodeKind_expr_stmt);
    stmt->node_expr = e;
    return stmt;
}

static AstNode* parse_block_stmt(Parser* p) {
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

static AstNode* parse_empty_stmt(Parser* p) {
    consume_token_if(p, TokenKind_semicolon);
    return ast_new(AstNodeKind_nop);
}

static AstNode* parse_stmt(Parser* p) {
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

static void register_params(Parser* p, AstNode* params) {
    for (int i = 0; i < params->node_len; ++i) {
        AstNode* param = params->node_items + i;
        add_lvar(p, param->name, param->ty, TRUE);
    }
}

static void register_func(Parser* p, const char* name, Type* ty) {
    Func* func = funcs_push_new(&p->funcs);
    func->name = name;
    func->ty = ty;
}

static AstNode* parse_param(Parser* p) {
    Type* ty = parse_type_name(p);
    const Token* name = NULL;
    TokenKind tk = peek_token(p)->kind;
    if (tk != TokenKind_comma && tk != TokenKind_paren_r) {
        name = parse_ident(p);
    }
    AstNode* param = ast_new(AstNodeKind_param);
    param->ty = ty;
    if (name) {
        param->name = name->value.string;
    }
    return param;
}

static AstNode* parse_param_list(Parser* p) {
    BOOL has_void = FALSE;
    AstNode* list = ast_new_list(6);
    while (peek_token(p)->kind != TokenKind_paren_r) {
        if (consume_token_if(p, TokenKind_ellipsis)) {
            break;
        }
        AstNode* param = parse_param(p);
        has_void = has_void || param->ty->kind == TypeKind_void;
        ast_append(list, param);
        if (!consume_token_if(p, TokenKind_comma)) {
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

static AstNode* parse_global_var_decl(Parser* p, AstNode* decls) {
    expect(p, TokenKind_semicolon);

    for (int i = 0; i < decls->node_len; ++i) {
        AstNode* decl = &decls->node_items[i];

        if (find_gvar(p, decl->name) != -1) {
            fatal_error("parse_global_var_decl: %s redeclared", decl->name);
        }
        GlobalVar* gvar = gvars_push_new(&p->gvars);
        gvar->name = decl->name;
        gvar->ty = decl->ty;

        decl->kind = AstNodeKind_gvar_decl;
        decl->node_expr = decl->node_init;
    }

    return decls;
}

static AstNode* parse_func_decl_or_def(Parser* p, AstNode* decls) {
    if (decls->node_len != 1) {
        fatal_error("parse_func_decl_or_def: invalid syntax");
    }
    Type* ty = decls->node_items[0].ty;
    const char* name = decls->node_items[0].name;

    register_func(p, name, ty);
    AstNode* params = parse_param_list(p);
    expect(p, TokenKind_paren_r);
    if (consume_token_if(p, TokenKind_semicolon)) {
        return ast_new(AstNodeKind_func_decl);
    }
    enter_func(p);
    register_params(p, params);
    AstNode* body = parse_block_stmt(p);
    leave_func(p);
    AstNode* func = ast_new(AstNodeKind_func_def);
    func->ty = ty;
    func->name = name;
    func->node_params = params;
    func->node_body = body;
    if (p->lvars.len == 0) {
        func->node_stack_size = 0;
    } else {
        func->node_stack_size =
            p->lvars.data[p->lvars.len - 1].stack_offset + type_sizeof(p->lvars.data[p->lvars.len - 1].ty);
    }
    func->node_function_is_static = ty->storage_class == StorageClass_static;
    return func;
}

static AstNode* parse_struct_member(Parser* p) {
    Type* ty = parse_type_name(p);
    const Token* name = parse_ident(p);
    expect(p, TokenKind_semicolon);
    AstNode* member = ast_new(AstNodeKind_struct_member);
    member->name = name->value.string;
    member->ty = ty;
    return member;
}

static AstNode* parse_struct_members(Parser* p) {
    AstNode* list = ast_new_list(4);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        AstNode* member = parse_struct_member(p);
        ast_append(list, member);
    }
    return list;
}

static AstNode* parse_union_member(Parser* p) {
    Type* ty = parse_type_name(p);
    const Token* name = parse_ident(p);
    expect(p, TokenKind_semicolon);
    AstNode* member = ast_new(AstNodeKind_union_member);
    member->name = name->value.string;
    member->ty = ty;
    return member;
}

static AstNode* parse_union_members(Parser* p) {
    AstNode* list = ast_new_list(4);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        AstNode* member = parse_union_member(p);
        ast_append(list, member);
    }
    return list;
}

static AstNode* parse_enum_member(Parser* p) {
    const Token* name = parse_ident(p);
    AstNode* member = ast_new(AstNodeKind_enum_member);
    member->name = name->value.string;

    if (consume_token_if(p, TokenKind_assign)) {
        // TODO: support other kinds of constant expression.
        const Token* v = expect(p, TokenKind_literal_int);
        member->node_int_value = v->value.integer;
    } else {
        // TODO: use another special value for placeholder because -1 is a valid value.
        member->node_int_value = -1;
    }

    return member;
}

static AstNode* parse_enum_members(Parser* p) {
    int next_value = 0;
    AstNode* list = ast_new_list(16);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        AstNode* member = parse_enum_member(p);

        if (member->node_int_value != -1) {
            next_value = member->node_int_value;
        } else {
            member->node_int_value = next_value;
        }
        ++next_value;

        ast_append(list, member);
        if (!consume_token_if(p, TokenKind_comma)) {
            break;
        }
    }
    return list;
}

void parse_typedef_decl(Parser* p, AstNode* decls) {
    expect(p, TokenKind_semicolon);
    for (int i = 0; i < decls->node_len; ++i) {
        AstNode* decl = &decls->node_items[i];

        AstNode* typedef_ = ast_new(AstNodeKind_typedef_decl);
        typedef_->name = decl->name;
        typedef_->ty = decl->ty;
        ast_append(p->typedefs, typedef_);
    }
}

void parse_extern_var_decl(Parser* p, AstNode* decls) {
    expect(p, TokenKind_semicolon);

    for (int i = 0; i < decls->node_len; ++i) {
        AstNode* decl = &decls->node_items[i];

        if (find_gvar(p, decl->name) != -1) {
            fatal_error("parse_extern_var_decl: %s redeclared", decl->name);
        }
        GlobalVar* gvar = gvars_push_new(&p->gvars);
        gvar->name = decl->name;
        gvar->ty = decl->ty;
    }
}

// struct-specifier:
//     'struct' TODO attribute-specifier-sequence? identifier? '{' member-declaration-list '}'
//     'struct' TODO attribute-specifier-sequence? identifier
static Type* parse_struct_specifier(Parser* p) {
    next_token(p);

    // TODO: support anonymous struct
    const Token* name = parse_ident(p);

    if (!consume_token_if(p, TokenKind_brace_l)) {
        int struct_idx = find_struct(p, name->value.string);
        if (struct_idx != -1) {
            Type* ty = type_new(TypeKind_struct);
            ty->ref.defs = p->structs;
            ty->ref.index = struct_idx;
            return ty;
        } else {
            // TODO
            AstNode* new_struct = ast_new(AstNodeKind_struct_def);
            new_struct->name = name->value.string;
            ast_append(p->structs, new_struct);
            struct_idx = p->structs->node_len - 1;

            Type* ty = type_new(TypeKind_struct);
            ty->ref.defs = p->structs;
            ty->ref.index = struct_idx;
            return ty;
        }
    }

    int struct_idx = find_struct(p, name->value.string);
    if (struct_idx != -1 && p->structs->node_items[struct_idx].node_members) {
        fatal_error("%s:%d: struct %s redefined", name->loc.filename, name->loc.line, name->value.string);
    }

    if (struct_idx == -1) {
        AstNode* new_struct = ast_new(AstNodeKind_struct_def);
        new_struct->name = name->value.string;
        ast_append(p->structs, new_struct);
        struct_idx = p->structs->node_len - 1;
    }

    AstNode* members = parse_struct_members(p);
    expect(p, TokenKind_brace_r);
    expect(p, TokenKind_semicolon);
    p->structs->node_items[struct_idx].node_members = members;

    Type* ty = type_new(TypeKind_struct);
    ty->ref.defs = p->structs;
    ty->ref.index = struct_idx;
    return ty;
}

// union-specifier:
//     'union' TODO attribute-specifier-sequence? identifier? '{' member-declaration-list '}'
//     'union' TODO attribute-specifier-sequence? identifier
static Type* parse_union_specifier(Parser* p) {
    next_token(p);

    // TODO: support anonymous union
    const Token* name = parse_ident(p);

    if (!consume_token_if(p, TokenKind_brace_l)) {
        int union_idx = find_union(p, name->value.string);
        if (union_idx != -1) {
            Type* ty = type_new(TypeKind_union);
            ty->ref.defs = p->unions;
            ty->ref.index = union_idx;
            return ty;
        } else {
            // TODO
            AstNode* new_union = ast_new(AstNodeKind_union_def);
            new_union->name = name->value.string;
            ast_append(p->unions, new_union);
            union_idx = p->unions->node_len - 1;

            Type* ty = type_new(TypeKind_union);
            ty->ref.defs = p->unions;
            ty->ref.index = union_idx;
            return ty;
        }
    }

    int union_idx = find_union(p, name->value.string);
    if (union_idx != -1 && p->unions->node_items[union_idx].node_members) {
        fatal_error("%s:%d: union %s redefined", name->loc.filename, name->loc.line, name->value.string);
    }

    if (union_idx == -1) {
        AstNode* new_union = ast_new(AstNodeKind_union_def);
        new_union->name = name->value.string;
        ast_append(p->unions, new_union);
        union_idx = p->unions->node_len - 1;
    }

    AstNode* members = parse_union_members(p);
    expect(p, TokenKind_brace_r);
    expect(p, TokenKind_semicolon);
    p->unions->node_items[union_idx].node_members = members;

    Type* ty = type_new(TypeKind_union);
    ty->ref.defs = p->unions;
    ty->ref.index = union_idx;
    return ty;
}

// enum-specifier:
//     'enum' TODO attribute-specifier-sequence? identifier? TODO enum-type-specifier? '{' enumerator-list ','? '}'
//     'enum' identifier TODO enum-type-specifier?
static Type* parse_enum_specifier(Parser* p) {
    next_token(p);

    // TODO: support anonymous enum
    const Token* name = parse_ident(p);

    if (!consume_token_if(p, TokenKind_brace_l)) {
        int enum_idx = find_enum(p, name->value.string);
        if (enum_idx != -1) {
            Type* ty = type_new(TypeKind_enum);
            ty->ref.defs = p->enums;
            ty->ref.index = enum_idx;
            return ty;
        } else {
            // TODO
            AstNode* new_enum = ast_new(AstNodeKind_enum_def);
            new_enum->name = name->value.string;
            ast_append(p->enums, new_enum);
            enum_idx = p->enums->node_len - 1;

            Type* ty = type_new(TypeKind_enum);
            ty->ref.defs = p->enums;
            ty->ref.index = enum_idx;
            return ty;
        }
    }

    int enum_idx = find_enum(p, name->value.string);
    if (enum_idx != -1 && p->enums->node_items[enum_idx].node_members) {
        fatal_error("%s:%d: enum %s redefined", name->loc.filename, name->loc.line, name->value.string);
    }

    if (enum_idx == -1) {
        AstNode* new_enum = ast_new(AstNodeKind_enum_def);
        new_enum->name = name->value.string;
        ast_append(p->enums, new_enum);
        enum_idx = p->enums->node_len - 1;
    }

    AstNode* members = parse_enum_members(p);
    expect(p, TokenKind_brace_r);
    p->enums->node_items[enum_idx].node_members = members;

    Type* ty = type_new(TypeKind_enum);
    ty->ref.defs = p->enums;
    ty->ref.index = enum_idx;
    return ty;
}

// declaration-specifiers:
//     { declaration-specifier }+ TODO attribute-specifier-sequence?
//
// declaration-specifier:
//     storage-class-specifier
//     type-specifier-qualifier
//     function-speicifier
//
// storage-class-specifier:
//     'auto'
//     'constexpr'
//     'extern'
//     'register'
//     'static'
//     'thread_local'
//     'typedef'
//
// type-specifier-qualifier:
//     type-specifier
//     type-qualifier
//     alignment-specifier
//
// function-specifier:
//     'inline'
//     '_Noreturn'
//
// type-specifier:
//     'void'
//     'char'
//     'short'
//     'int'
//     'long'
//     'float'
//     'double'
//     'signed'
//     'unsigned'
//     TODO '_BitInt' '(' constant-expression ')'
//     'bool'
//     '_Complex'
//     '_Decimal32'
//     '_Decimal64'
//     '_Decimal128'
//     atomic-type-specifier
//     struct-or-union-specifier
//     enum-specifier
//     typeof-specifier
//     typedef-name
//
// type-qualifier:
//     'const'
//     'restrict'
//     'volatile'
//     '_Atomic'
//
// alignment-specifier:
//     TODO 'alignas' '(' type-name ')'
//     TODO 'alignas' '(' constant-expression ')'
//
// atomic-type-specifier:
//     TODO '_Atomic' '(' type-name ')'
//
// struct-or-union-specifier:
//     'struct' ...
//     'union' ...
//
// enum-specifier:
//     'enum' ...
//
// typeof-specifier:
//     TODO 'typeof' '(' typeof-specifier-argument ')'
//     TODO 'typeof_unqual' '(' typeof-specifier-argument ')'
//
// typedef-name:
//     identifier
static Type* parse_declaration_specifiers(Parser* p) {
    StorageClass storage_class = StorageClass_unspecified;
    Type* ty = NULL;

    while (1) {
        Token* tok = peek_token(p);

        // storage-class-spciefier
        if (tok->kind == TokenKind_keyword_auto) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_constexpr) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_extern) {
            next_token(p);
            storage_class = StorageClass_extern;
        } else if (tok->kind == TokenKind_keyword_register) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_static) {
            next_token(p);
            storage_class = StorageClass_static;
        } else if (tok->kind == TokenKind_keyword_thread_local) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_typedef) {
            next_token(p);
            storage_class = StorageClass_typedef;
        }
        // type-specifier-qualifier > type-specifier
        else if (tok->kind == TokenKind_keyword_void) {
            next_token(p);
            ty = type_new(TypeKind_void);
        } else if (tok->kind == TokenKind_keyword_char) {
            next_token(p);
            ty = type_new(TypeKind_char);
        } else if (tok->kind == TokenKind_keyword_short) {
            next_token(p);
            ty = type_new(TypeKind_short);
        } else if (tok->kind == TokenKind_keyword_int) {
            next_token(p);
            ty = type_new(TypeKind_int);
        } else if (tok->kind == TokenKind_keyword_long) {
            next_token(p);
            ty = type_new(TypeKind_long);
        } else if (tok->kind == TokenKind_keyword_float) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_double) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_signed) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_unsigned) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__BitInt) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_bool) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Complex) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Decimal32) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Decimal64) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Decimal128) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Atomic) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_struct) {
            ty = parse_struct_specifier(p);
        } else if (tok->kind == TokenKind_keyword_union) {
            ty = parse_union_specifier(p);
        } else if (tok->kind == TokenKind_keyword_enum) {
            ty = parse_enum_specifier(p);
        } else if (tok->kind == TokenKind_keyword_typeof) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_typeof_unqual) {
            unimplemented();
        } else if (is_typedef_name(p, tok)) {
            next_token(p);
            int typedef_idx = find_typedef(p, tok->value.string);
            ty = p->typedefs->node_items[typedef_idx].ty;
        }
        // type-specifier-qualifier > type-qualifier
        else if (tok->kind == TokenKind_keyword_const) {
            // TODO
            next_token(p);
        } else if (tok->kind == TokenKind_keyword_restrict) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_volatile) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Atomic) {
            unimplemented();
        }
        // type-specifier-qualifier > alignment-specifier
        else if (tok->kind == TokenKind_keyword_alignas) {
            unimplemented();
        }
        // function-specifier
        else if (tok->kind == TokenKind_keyword_inline) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Noreturn) {
            // TODO
            next_token(p);
        } else {
            break;
        }
    }

    ty->storage_class = storage_class;
    return ty;
}

// specifier-qualifier-list:
//     { type-specifier-qualifier }+ TODO attribute-specifier-sequence?
static Type* parse_specifier_qualifier_list(Parser* p) {
    Type* ty = NULL;

    while (1) {
        Token* tok = peek_token(p);

        // type-specifier-qualifier > type-specifier
        if (tok->kind == TokenKind_keyword_void) {
            next_token(p);
            ty = type_new(TypeKind_void);
        } else if (tok->kind == TokenKind_keyword_char) {
            next_token(p);
            ty = type_new(TypeKind_char);
        } else if (tok->kind == TokenKind_keyword_short) {
            next_token(p);
            ty = type_new(TypeKind_short);
        } else if (tok->kind == TokenKind_keyword_int) {
            next_token(p);
            ty = type_new(TypeKind_int);
        } else if (tok->kind == TokenKind_keyword_long) {
            next_token(p);
            ty = type_new(TypeKind_long);
        } else if (tok->kind == TokenKind_keyword_float) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_double) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_signed) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_unsigned) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__BitInt) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_bool) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Complex) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Decimal32) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Decimal64) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Decimal128) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Atomic) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_struct) {
            ty = parse_struct_specifier(p);
        } else if (tok->kind == TokenKind_keyword_union) {
            ty = parse_union_specifier(p);
        } else if (tok->kind == TokenKind_keyword_enum) {
            ty = parse_enum_specifier(p);
        } else if (tok->kind == TokenKind_keyword_typeof) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_typeof_unqual) {
            unimplemented();
        } else if (is_typedef_name(p, tok)) {
            next_token(p);
            int typedef_idx = find_typedef(p, tok->value.string);
            ty = p->typedefs->node_items[typedef_idx].ty;
        }
        // type-specifier-qualifier > type-qualifier
        else if (tok->kind == TokenKind_keyword_const) {
            // TODO
            next_token(p);
        } else if (tok->kind == TokenKind_keyword_restrict) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_volatile) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword__Atomic) {
            unimplemented();
        }
        // type-specifier-qualifier > alignment-specifier
        else if (tok->kind == TokenKind_keyword_alignas) {
            unimplemented();
        } else {
            break;
        }
    }

    return ty;
}

// abstract-declarator:
//     pointer
//     pointer? TODO direct-abstract-declarator
//
// abstract-declarator?:
//     pointer? TODO direct-abstract-declarator?
static Type* parse_abstract_declarator_opt(Parser* p, Type* ty) {
    return parse_pointer_opt(p, ty);
}

// type-name:
//     specifier-qualifier-list abstract-declarator?
static Type* parse_type_name(Parser* p) {
    Type* ty = parse_specifier_qualifier_list(p);
    ty = parse_abstract_declarator_opt(p, ty);
    return ty;
}

static AstNode* parse_toplevel(Parser* p) {
    Type* ty = parse_declaration_specifiers(p);
    if (consume_token_if(p, TokenKind_semicolon)) {
        // Type declaration.
        return NULL;
    }

    AstNode* decls = parse_init_declarator_list(p, ty);

    if (consume_token_if(p, TokenKind_paren_l)) {
        return parse_func_decl_or_def(p, decls);
    }

    if (ty->storage_class == StorageClass_typedef) {
        parse_typedef_decl(p, decls);
        return NULL;
    } else if (ty->storage_class == StorageClass_extern) {
        parse_extern_var_decl(p, decls);
        return NULL;
    } else {
        return parse_global_var_decl(p, decls);
    }
}

Program* parse(TokenArray* tokens) {
    Parser* p = parser_new(tokens);
    AstNode* funcs = ast_new_list(32);
    AstNode* vars = ast_new_list(16);
    while (eof(p)) {
        AstNode* n = parse_toplevel(p);
        if (!n)
            continue;
        if (n->kind == AstNodeKind_func_def) {
            ast_append(funcs, n);
        } else if (n->kind == AstNodeKind_gvar_decl && n->ty) {
            ast_append(vars, n);
        } else if (n->kind == AstNodeKind_list) {
            for (int i = 0; i < n->node_len; ++i) {
                ast_append(vars, &n->node_items[i]);
            }
        }
    }
    Program* prog = calloc(1, sizeof(Program));
    prog->funcs = funcs;
    prog->vars = vars;
    prog->str_literals = p->str_literals.data;
    return prog;
}

static int eval(AstNode* e) {
    if (e->kind == AstNodeKind_int_expr) {
        return e->node_int_value;
    } else if (e->kind == AstNodeKind_unary_expr) {
        int v = eval(e->node_operand);
        if (e->node_op == TokenKind_not) {
            return !v;
        } else if (e->node_op == TokenKind_minus) {
            return -v;
        } else {
            unimplemented();
        }
    } else if (e->kind == AstNodeKind_binary_expr || e->kind == AstNodeKind_logical_expr) {
        int v1 = eval(e->node_lhs);
        int v2 = eval(e->node_rhs);
        if (e->node_op == TokenKind_andand) {
            return v1 && v2;
        } else if (e->node_op == TokenKind_oror) {
            return v1 || v2;
        } else if (e->node_op == TokenKind_plus) {
            return v1 + v2;
        } else if (e->node_op == TokenKind_minus) {
            return v1 - v2;
        } else if (e->node_op == TokenKind_star) {
            return v1 * v2;
        } else if (e->node_op == TokenKind_slash) {
            if (v2 == 0) {
                fatal_error("eval: division by zero");
            }
            return v1 / v2;
        } else if (e->node_op == TokenKind_percent) {
            if (v2 == 0) {
                fatal_error("eval: division by zero");
            }
            return v1 % v2;
        } else if (e->node_op == TokenKind_eq) {
            return v1 == v2;
        } else if (e->node_op == TokenKind_ne) {
            return v1 != v2;
        } else if (e->node_op == TokenKind_lt) {
            return v1 < v2;
        } else if (e->node_op == TokenKind_le) {
            return v1 <= v2;
        } else if (e->node_op == TokenKind_lshift) {
            return v1 << v2;
        } else if (e->node_op == TokenKind_rshift) {
            return v1 >> v2;
        } else {
            unimplemented();
        }
    } else if (e->kind == AstNodeKind_cond_expr) {
        int cond = eval(e->node_cond);
        if (cond) {
            return eval(e->node_then);
        } else {
            return eval(e->node_else);
        }
    } else {
        unimplemented();
    }
}

BOOL pp_eval_constant_expression(TokenArray* pp_tokens) {
    TokenArray* tokens = tokenize(pp_tokens);
    Parser* p = parser_new(tokens);
    AstNode* e = parse_constant_expression(p);
    return eval(e) != 0;
}
