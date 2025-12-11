#include "parse.h"
#include "common.h"
#include "tokenize.h"

typedef struct {
    const char* name;
    Type* ty;
    int stack_offset;
} LocalVar;

typedef struct {
    size_t len;
    size_t capacity;
    LocalVar* data;
} LocalVarArray;

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

typedef struct {
    const char* name;
    int index;
} ScopedSymbol;

typedef struct {
    size_t len;
    size_t capacity;
    ScopedSymbol* data;
} ScopedSymbolArray;

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

typedef struct Scope {
    struct Scope* outer;
    ScopedSymbolArray syms;
} Scope;

typedef struct {
    const char* name;
    Type* ty;
} GlobalVar;

typedef struct {
    size_t len;
    size_t capacity;
    GlobalVar* data;
} GlobalVarArray;

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

typedef struct {
    const char* name;
    Type* ty;
} Func;

typedef struct {
    size_t len;
    size_t capacity;
    Func* data;
} FuncArray;

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

typedef struct {
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
    int anonymous_user_type_counter;
    AstNode* current_switch;
} Parser;

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

static bool eof(Parser* p) {
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

static int calc_stack_offset(Parser* p, Type* ty, bool is_param) {
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

static int add_lvar(Parser* p, const char* name, Type* ty, bool is_param) {
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
    int stack_offset = add_lvar(p, NULL, ty, false);
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
    } else if (t->kind == TokenKind_keyword_true) {
        return ast_new_int(1);
    } else if (t->kind == TokenKind_keyword_false) {
        return ast_new_int(0);
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
            e->ty = p->funcs.data[func_idx].ty->result;
            return e;
        }

        int lvar_idx = find_lvar(p, name);
        if (lvar_idx == -1) {
            int gvar_idx = find_gvar(p, name);
            if (gvar_idx == -1) {
                int enum_member_idx = find_enum_member(p, name);
                if (enum_member_idx == -1) {
                    int func_idx = find_func(p, name);
                    if (func_idx == -1) {
                        fatal_error("undefined variable: %s", name);
                    }
                    AstNode* e = ast_new(AstNodeKind_func);
                    e->name = name;
                    e->ty = p->funcs.data[func_idx].ty;
                    return e;
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

static bool is_typedef_name(Parser* p, Token* tok) {
    return tok->kind == TokenKind_ident && find_typedef(p, tok->value.string) != -1;
}

static bool is_type_token(Parser* p, Token* tok) {
    if (tok->kind == TokenKind_keyword_void || tok->kind == TokenKind_keyword_char ||
        tok->kind == TokenKind_keyword_short || tok->kind == TokenKind_keyword_int ||
        tok->kind == TokenKind_keyword_long || tok->kind == TokenKind_keyword_float ||
        tok->kind == TokenKind_keyword_double || tok->kind == TokenKind_keyword_signed ||
        tok->kind == TokenKind_keyword_unsigned || tok->kind == TokenKind_keyword__BitInt ||
        tok->kind == TokenKind_keyword_bool || tok->kind == TokenKind_keyword__Complex ||
        tok->kind == TokenKind_keyword__Decimal32 || tok->kind == TokenKind_keyword__Decimal64 ||
        tok->kind == TokenKind_keyword__Decimal128 || tok->kind == TokenKind_keyword__Atomic ||
        tok->kind == TokenKind_keyword_struct || tok->kind == TokenKind_keyword_union ||
        tok->kind == TokenKind_keyword_enum || tok->kind == TokenKind_keyword_typeof ||
        tok->kind == TokenKind_keyword_typeof_unqual || tok->kind == TokenKind_keyword_const ||
        tok->kind == TokenKind_keyword_restrict || tok->kind == TokenKind_keyword_volatile ||
        tok->kind == TokenKind_keyword__Atomic || tok->kind == TokenKind_keyword_alignas ||
        tok->kind == TokenKind_keyword_inline || tok->kind == TokenKind_keyword__Noreturn) {
        return true;
    }
    if (tok->kind != TokenKind_ident) {
        return false;
    }
    return find_typedef(p, tok->value.string) != -1;
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
    } else if (consume_token_if(p, TokenKind_tilde)) {
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
        return ast_new_cast_expr(e, ty);
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

static AstNode* parse_bitwise_and_expr(Parser* p) {
    AstNode* lhs = parse_equality_expr(p);
    while (1) {
        if (consume_token_if(p, TokenKind_and)) {
            AstNode* rhs = parse_equality_expr(p);
            lhs = ast_new_binary_expr(TokenKind_and, lhs, rhs);
            lhs->ty = type_new(TypeKind_int);
        } else {
            break;
        }
    }
    return lhs;
}

static AstNode* parse_bitwise_xor_expr(Parser* p) {
    AstNode* lhs = parse_bitwise_and_expr(p);
    while (1) {
        if (consume_token_if(p, TokenKind_xor)) {
            AstNode* rhs = parse_bitwise_and_expr(p);
            lhs = ast_new_binary_expr(TokenKind_xor, lhs, rhs);
            lhs->ty = type_new(TypeKind_int);
        } else {
            break;
        }
    }
    return lhs;
}

static AstNode* parse_bitwise_or_expr(Parser* p) {
    AstNode* lhs = parse_bitwise_xor_expr(p);
    while (1) {
        if (consume_token_if(p, TokenKind_or)) {
            AstNode* rhs = parse_bitwise_xor_expr(p);
            lhs = ast_new_binary_expr(TokenKind_or, lhs, rhs);
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
        if (consume_token_if(p, TokenKind_andand)) {
            AstNode* rhs = parse_bitwise_or_expr(p);
            AstNode* e = ast_new(AstNodeKind_logical_expr);
            e->node_op = TokenKind_andand;
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
        if (consume_token_if(p, TokenKind_oror)) {
            AstNode* rhs = parse_logical_and_expr(p);
            AstNode* e = ast_new(AstNodeKind_logical_expr);
            e->node_op = TokenKind_oror;
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
            op == TokenKind_assign_mod || op == TokenKind_assign_or || op == TokenKind_assign_and ||
            op == TokenKind_assign_xor || op == TokenKind_assign_lshift || op == TokenKind_assign_rshift) {
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

// type-qualifier-list:
//     { type-qualifier }+
static void parse_type_qualifier_list_opt(Parser* p) {
    while (1) {
        Token* tok = peek_token(p);
        if (tok->kind == TokenKind_keyword_const || tok->kind == TokenKind_keyword_restrict ||
            tok->kind == TokenKind_keyword_volatile || tok->kind == TokenKind_keyword__Atomic) {
            // TODO: currently type qualifiers are just ignored.
            next_token(p);
        } else {
            break;
        }
    }
}

// pointer:
//     { '*' TODO attribute-specifier-sequence? type-qualifier-list? }+
static Type* parse_pointer_opt(Parser* p, Type* ty) {
    while (peek_token(p)->kind == TokenKind_star) {
        next_token(p);
        ty = type_new_ptr(ty);
        parse_type_qualifier_list_opt(p);
    }
    return ty;
}

static Type* parse_array_declarator_suffix(Parser* p, Type* ty);
static Type* parse_function_declarator_suffix(Parser* p, Type* ty);

// declarator | abstract-declarator?:
//     pointer? identifier TODO attribute-specifier-sequence? { direct-declarator-suffix }*
//     pointer? '(' declarator ')' { direct-declarator-suffix }*
//     pointer? '(' abstract-declarator ')' { direct-declarator-suffix }*
//     pointer? { direct-declarator-suffix }*
//
// direct-declarator-suffix:
//     array-declarator-suffix TODO attribute-specifier-sequence?
//     function-declarator-suffix TODO attribute-specifier-sequence?
static AstNode* parse_declarator_or_abstract_declarator_opt(Parser* p, Type* ty) {
    ty = parse_pointer_opt(p, ty);

    AstNode* decl;
    if (peek_token(p)->kind == TokenKind_ident) {
        decl = ast_new(AstNodeKind_declarator);
        decl->name = parse_ident(p)->value.string;
        decl->ty = ty;
    } else if (peek_token(p)->kind == TokenKind_paren_l && !is_type_token(p, peek_token2(p))) {
        next_token(p);
        if (peek_token(p)->kind == TokenKind_paren_r) {
            Token* tok = peek_token(p);
            fatal_error("%s:%d: expected declarator, but got '%s'", tok->loc.filename, tok->loc.line,
                        token_stringify(tok));
        }
        decl = parse_declarator_or_abstract_declarator_opt(p, ty);
        expect(p, TokenKind_paren_r);
    } else {
        decl = ast_new(AstNodeKind_declarator);
        decl->ty = ty;
    }

    while (1) {
        if (peek_token(p)->kind == TokenKind_bracket_l) {
            decl->ty = parse_array_declarator_suffix(p, decl->ty);
        } else if (peek_token(p)->kind == TokenKind_paren_l) {
            decl->ty = parse_function_declarator_suffix(p, decl->ty);
        } else {
            break;
        }
    }

    return decl;
}

static Type* parse_declaration_specifiers(Parser* p);

// parameter-declaration:
//     TODO attribute-specifier-sequence? declaration-specifiers declarator
//     TODO attribute-specifier-sequence? declaration-specifiers abstract-declarator?
static AstNode* parse_parameter_declaration(Parser* p) {
    if (consume_token_if(p, TokenKind_ellipsis)) {
        AstNode* decl = ast_new(AstNodeKind_declarator);
        decl->name = "...";
        return decl;
    }

    Type* base_ty = parse_declaration_specifiers(p);
    return parse_declarator_or_abstract_declarator_opt(p, base_ty);
}

// parameter-list:
//     { parameter-declaration | ',' }
static AstNode* parse_parameter_list(Parser* p) {
    AstNode* params = ast_new_list(4);

    while (1) {
        AstNode* param = parse_parameter_declaration(p);
        ast_append(params, param);

        if (!consume_token_if(p, TokenKind_comma)) {
            break;
        }
    }

    return params;
}

// parameter-type-list:
//     parameter-list
//     parameter-list ',' '...'
//     '...'
static AstNode* parse_parameter_type_list(Parser* p) {
    AstNode* params = parse_parameter_list(p);

    bool has_void = false;
    for (int i = 0; i < params->node_len; ++i) {
        if (params->node_items[i].name && strcmp(params->node_items[i].name, "...") == 0) {
            if (i != params->node_len - 1) {
                fatal_error("...");
            }
            --params->node_len;
            break;
        }
        has_void |= params->node_items[i].ty->kind == TypeKind_void;
    }

    if (params->node_len > 6) {
        fatal_error("too many parameters");
    }
    if (has_void) {
        if (params->node_len != 1) {
            fatal_error("invalid use of void param");
        }
        params->node_len = 0;
    }

    return params;
}

static int eval(AstNode* e);

// array-declarator:
//     direct-declarator '[' TODO type-qualifier-list? TODO assignment-expression? ']'
//     TODO direct-declarator '[' 'static' type-qualifier-list? assignment-expression? ']'
//     TODO direct-declarator '[' type-qualifier-list 'static' assignment-expression? ']'
//     TODO direct-declarator '[' type-qualifier-list? '*' ']'
static Type* parse_array_declarator_suffix(Parser* p, Type* ty) {
    next_token(p); // skip '['

    int size = -1;
    if (peek_token(p)->kind != TokenKind_bracket_r) {
        AstNode* size_expr = parse_assignment_expr(p);
        size = eval(size_expr);
    }
    expect(p, TokenKind_bracket_r);

    return type_new_array(ty, size);
}

// function-declarator:
//     direct-declarator '(' parameter-type-list? ')'
static Type* parse_function_declarator_suffix(Parser* p, Type* ty) {
    next_token(p); // skip '('
    AstNode* params;

    // FIXME: save and restore ty->storage_class because it will be modified somewhere for some reason.
    StorageClass FIXME_storage_class = ty->storage_class;
    if (consume_token_if(p, TokenKind_paren_r)) {
        params = ast_new_list(1);
    } else {
        params = parse_parameter_type_list(p);
        expect(p, TokenKind_paren_r);
    }
    ty->storage_class = FIXME_storage_class;

    return type_new_func(ty, params);
}

static AstNode* parse_declarator(Parser* p, Type* ty);

// direct-declarator:
//     identifier TODO attribute-specifier-sequence?
//     '(' declarator ')'
//     array-declarator TODO attribute-specifier-sequence?
//     function-declarator TODO attribute-specifier-sequence?
static AstNode* parse_direct_declarator(Parser* p, Type* ty) {
    AstNode* decl;
    if (peek_token(p)->kind == TokenKind_ident) {
        decl = ast_new(AstNodeKind_declarator);
        decl->name = parse_ident(p)->value.string;
        decl->ty = ty;
    } else if (peek_token(p)->kind == TokenKind_paren_l && !is_type_token(p, peek_token2(p))) {
        next_token(p);
        decl = parse_declarator(p, ty);
        expect(p, TokenKind_paren_r);
    } else {
        decl = ast_new(AstNodeKind_declarator);
        decl->ty = ty;
    }

    while (1) {
        if (peek_token(p)->kind == TokenKind_bracket_l) {
            decl->ty = parse_array_declarator_suffix(p, decl->ty);
        } else if (peek_token(p)->kind == TokenKind_paren_l) {
            decl->ty = parse_function_declarator_suffix(p, decl->ty);
        } else {
            break;
        }
    }

    AstNode* ret = ast_new(AstNodeKind_declarator);
    ret->name = decl->name;
    ret->ty = decl->ty;
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

static void declare_func_or_var(Parser* p, AstNode* decl);

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

        if (ty->storage_class == StorageClass_typedef) {
            continue;
        }
        // Immediately declare to allow following initializer to access previous variables. For example,
        //   int a = 1, b = a;
        declare_func_or_var(p, &list->node_items[list->node_len - 1]);
    }
    return list;
}

static AstNode* parse_var_decl(Parser* p) {
    Type* base_ty = parse_type_name(p);
    AstNode* decls = parse_init_declarator_list(p, base_ty);
    expect(p, TokenKind_semicolon);
    declare_func_or_var(p, &decls->node_items[decls->node_len - 1]);
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

static AstNode* parse_switch_stmt(Parser* p) {
    expect(p, TokenKind_keyword_switch);
    expect(p, TokenKind_paren_l);
    AstNode* expr = parse_expr(p);
    expect(p, TokenKind_paren_r);

    AstNode* tmp_var = generate_temporary_lvar(p, expr->ty);
    AstNode* assignment = ast_new_assign_expr(TokenKind_assign, tmp_var, expr);
    AstNode* assign_stmt = ast_new(AstNodeKind_expr_stmt);
    assign_stmt->node_expr = assignment;

    AstNode* switch_stmt = ast_new(AstNodeKind_switch_stmt);
    switch_stmt->node_expr = tmp_var;

    AstNode* prev_switch = p->current_switch;
    p->current_switch = switch_stmt;
    switch_stmt->node_body = parse_stmt(p);
    p->current_switch = prev_switch;

    AstNode* list = ast_new_list(2);
    ast_append(list, assign_stmt);
    ast_append(list, switch_stmt);
    return list;
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

    if (t->kind == TokenKind_keyword_case) {
        if (!p->current_switch) {
            fatal_error("%s:%d: 'case' label not within a switch statement", t->loc.filename, t->loc.line);
        }
        expect(p, TokenKind_keyword_case);
        AstNode* value = parse_constant_expression(p);
        expect(p, TokenKind_colon);
        AstNode* stmt = parse_stmt(p);

        AstNode* case_label = ast_new(AstNodeKind_case_label);
        case_label->node_int_value = eval(value);
        case_label->node_body = stmt;
        return case_label;
    } else if (t->kind == TokenKind_keyword_default) {
        if (!p->current_switch) {
            fatal_error("%s:%d: 'default' label not within a switch statement", t->loc.filename, t->loc.line);
        }
        expect(p, TokenKind_keyword_default);
        expect(p, TokenKind_colon);
        AstNode* stmt = parse_stmt(p);

        AstNode* default_label = ast_new(AstNodeKind_default_label);
        default_label->node_body = stmt;
        return default_label;
    } else if (t->kind == TokenKind_keyword_return) {
        return parse_return_stmt(p);
    } else if (t->kind == TokenKind_keyword_if) {
        return parse_if_stmt(p);
    } else if (t->kind == TokenKind_keyword_switch) {
        return parse_switch_stmt(p);
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
    } else if (t->kind == TokenKind_keyword_goto) {
        expect(p, TokenKind_keyword_goto);
        Token* label_token = expect(p, TokenKind_ident);
        expect(p, TokenKind_semicolon);

        AstNode* goto_stmt = ast_new(AstNodeKind_goto_stmt);
        goto_stmt->name = label_token->value.string;
        return goto_stmt;
    } else if (t->kind == TokenKind_brace_l) {
        return parse_block_stmt(p);
    } else if (t->kind == TokenKind_semicolon) {
        return parse_empty_stmt(p);
    } else if (is_type_token(p, t)) {
        return parse_var_decl(p);
    } else if (t->kind == TokenKind_ident && peek_token2(p)->kind == TokenKind_colon) {
        // Label statement
        Token* label_token = expect(p, TokenKind_ident);
        expect(p, TokenKind_colon);
        AstNode* stmt = parse_stmt(p);

        AstNode* label_stmt = ast_new(AstNodeKind_label_stmt);
        label_stmt->name = label_token->value.string;
        label_stmt->node_body = stmt;
        return label_stmt;
    } else {
        return parse_expr_stmt(p);
    }
}

static void register_params(Parser* p, AstNode* params) {
    for (int i = 0; i < params->node_len; ++i) {
        AstNode* param = params->node_items + i;
        add_lvar(p, param->name, param->ty, true);
    }
}

static void register_func(Parser* p, const char* name, Type* ty) {
    Func* func = funcs_push_new(&p->funcs);
    func->name = name;
    func->ty = ty;
}

static void declare_func_or_var(Parser* p, AstNode* decl) {
    if (decl->ty->kind == TypeKind_func) {
        // TODO: refactor
        decl->ty->storage_class = decl->ty->result->storage_class;
        decl->ty->result->storage_class = StorageClass_unspecified;
        register_func(p, decl->name, decl->ty);
        decl->kind = AstNodeKind_func_decl;
    } else {
        if (type_is_unsized(decl->ty)) {
            fatal_error("declare_func_or_var: invalid type for variable");
        }

        if (p->scope) {
            if (find_lvar_in_current_scope(p, decl->name) != -1) {
                // TODO: use name's location.
                fatal_error("%s:%d: '%s' redeclared", peek_token(p)->loc.filename, peek_token(p)->loc.line, decl->name);
            }
            int stack_offset = add_lvar(p, decl->name, decl->ty, false);

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
        } else {
            if (find_gvar(p, decl->name) != -1) {
                fatal_error("declare_func_or_var: %s redeclared", decl->name);
            }
            // TODO: refactor
            Type* base_ty = decl->ty;
            while (base_ty->base) {
                base_ty = base_ty->base;
            }
            decl->ty->storage_class = base_ty->storage_class;
            base_ty->storage_class = StorageClass_unspecified;

            GlobalVar* gvar = gvars_push_new(&p->gvars);
            gvar->name = decl->name;
            gvar->ty = decl->ty;

            if (decl->ty->storage_class == StorageClass_extern) {
                decl->kind = AstNodeKind_nop;
            } else {
                decl->kind = AstNodeKind_gvar_decl;
                decl->node_expr = decl->node_init;
            }
        }
    }
}

static AstNode* parse_func_def(Parser* p, AstNode* decls) {
    if (decls->node_len != 1) {
        fatal_error("parse_func_def: invalid syntax");
    }

    Type* ty = decls->node_items[0].ty;
    Type* base_ty = ty->result;
    while (base_ty->base) {
        base_ty = base_ty->base;
    }
    ty->storage_class = base_ty->storage_class;
    base_ty->storage_class = StorageClass_unspecified;
    const char* name = decls->node_items[0].name;
    AstNode* params = ty->params;

    register_func(p, name, ty);
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
    return func;
}

// member-declarator:
//     declarator
//     TODO declarator? ':' constant-expression
static AstNode* parse_member_declarator(Parser* p, Type* base_ty) {
    return parse_declarator(p, base_ty);
}

// member-declarator-list:
//     { member-declarator | ',' }+
static AstNode* parse_member_declarator_list(Parser* p, Type* base_ty) {
    AstNode* list = ast_new_list(1);
    while (1) {
        AstNode* d = parse_member_declarator(p, base_ty);
        ast_append(list, d);
        if (!consume_token_if(p, TokenKind_comma)) {
            break;
        }
    }
    return list;
}

static Type* parse_specifier_qualifier_list(Parser* p);

// member-declaration:
//     TODO attribute-specifier-sequence? specifier-qualifier-list member-declaration-list? ';'
//     TODO static_assert-declaration
static AstNode* parse_member_declaration(Parser* p) {
    Type* base_ty = parse_specifier_qualifier_list(p);

    AstNode* decls = NULL;
    if (consume_token_if(p, TokenKind_semicolon)) {
        unimplemented();
    }
    decls = parse_member_declarator_list(p, base_ty);

    expect(p, TokenKind_semicolon);

    if (decls->node_len != 1)
        unimplemented();

    AstNode* m = ast_new(AstNodeKind_struct_member);
    m->name = decls->node_items[0].name;
    m->ty = decls->node_items[0].ty;
    return m;
}

// member-declaration-list:
//     { member-declaration }+
static AstNode* parse_member_declaration_list(Parser* p) {
    AstNode* list = ast_new_list(4);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        AstNode* member = parse_member_declaration(p);
        ast_append(list, member);
    }
    return list;
}

static AstNode* parse_enum_member(Parser* p) {
    const Token* name = parse_ident(p);
    AstNode* member = ast_new(AstNodeKind_enum_member);
    member->name = name->value.string;

    if (consume_token_if(p, TokenKind_assign)) {
        member->node_expr = parse_constant_expression(p);
    }

    return member;
}

static void parse_enum_members(Parser* p, int enum_idx) {
    int next_value = 0;
    AstNode* list = ast_new_list(16);

    if (!p->enums->node_items[enum_idx].node_members) {
        p->enums->node_items[enum_idx].node_members = list;
    }

    while (peek_token(p)->kind != TokenKind_brace_r) {
        AstNode* member = parse_enum_member(p);

        if (member->node_expr) {
            member->node_int_value = eval(member->node_expr);
            member->node_expr = NULL;
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

static char* generate_new_anonymous_user_type_name(Parser* p) {
    char* buf = calloc(32, sizeof(char));
    sprintf(buf, "__anonymous_%d__", p->anonymous_user_type_counter++);
    return buf;
}

// struct-specifier:
//     'struct' TODO attribute-specifier-sequence? identifier? '{' member-declaration-list '}'
//     'struct' TODO attribute-specifier-sequence? identifier
static Type* parse_struct_specifier(Parser* p) {
    SourceLocation struct_kw_pos = peek_token(p)->loc;
    next_token(p);

    const Token* name;
    char* anonymous_name = NULL;
    if (peek_token(p)->kind == TokenKind_brace_l) {
        anonymous_name = generate_new_anonymous_user_type_name(p);
        Token* anonymous_token = calloc(1, sizeof(Token));
        anonymous_token->kind = TokenKind_ident;
        anonymous_token->value.string = anonymous_name;
        anonymous_token->loc = struct_kw_pos;
        name = anonymous_token;
    } else {
        name = parse_ident(p);
    }

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

    AstNode* members = parse_member_declaration_list(p);
    expect(p, TokenKind_brace_r);
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
    SourceLocation union_kw_pos = peek_token(p)->loc;
    next_token(p);

    const Token* name;
    char* anonymous_name = NULL;
    if (peek_token(p)->kind == TokenKind_brace_l) {
        anonymous_name = generate_new_anonymous_user_type_name(p);
        Token* anonymous_token = calloc(1, sizeof(Token));
        anonymous_token->kind = TokenKind_ident;
        anonymous_token->value.string = anonymous_name;
        anonymous_token->loc = union_kw_pos;
        name = anonymous_token;
    } else {
        name = parse_ident(p);
    }

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

    AstNode* members = parse_member_declaration_list(p);
    expect(p, TokenKind_brace_r);
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
    SourceLocation enum_kw_pos = peek_token(p)->loc;
    next_token(p);

    const Token* name;
    char* anonymous_name = NULL;
    if (peek_token(p)->kind == TokenKind_brace_l) {
        anonymous_name = generate_new_anonymous_user_type_name(p);
        Token* anonymous_token = calloc(1, sizeof(Token));
        anonymous_token->kind = TokenKind_ident;
        anonymous_token->value.string = anonymous_name;
        anonymous_token->loc = enum_kw_pos;
        name = anonymous_token;
    } else {
        name = parse_ident(p);
    }

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

    parse_enum_members(p, enum_idx);
    expect(p, TokenKind_brace_r);

    Type* ty = type_new(TypeKind_enum);
    ty->ref.defs = p->enums;
    ty->ref.index = enum_idx;
    return ty;
}

typedef enum {
    TypeSpecifierMask_void = 1 << 0,
    TypeSpecifierMask_char = 1 << 1,
    TypeSpecifierMask_short = 1 << 2,
    TypeSpecifierMask_int = 1 << 3,
    TypeSpecifierMask_long = 1 << 4,
    // 1 << 5 is used for second 'long'.
    TypeSpecifierMask_float = 1 << 6,
    TypeSpecifierMask_double = 1 << 7,
    TypeSpecifierMask_signed = 1 << 8,
    TypeSpecifierMask_unsigned = 1 << 9,
    TypeSpecifierMask__BitInt = 1 << 10,
    TypeSpecifierMask_bool = 1 << 11,
    TypeSpecifierMask__Complex = 1 << 12,
    TypeSpecifierMask__Decimal32 = 1 << 13,
    TypeSpecifierMask__Decimal64 = 1 << 14,
    TypeSpecifierMask__Decimal128 = 1 << 15,
    TypeSpecifierMask__Atomic = 1 << 16,
    TypeSpecifierMask_struct = 1 << 17,
    TypeSpecifierMask_union = 1 << 18,
    TypeSpecifierMask_enum = 1 << 19,
    TypeSpecifierMask_typeof = 1 << 20,
    TypeSpecifierMask_typeof_unqual = 1 << 21,
    TypeSpecifierMask_typedef_name = 1 << 22,
} TypeSpecifierMask;

static Type* distinguish_type_from_type_specifiers(int type_specifiers) {
    if (type_specifiers == TypeSpecifierMask_void)
        return type_new(TypeKind_void);
    else if (type_specifiers == TypeSpecifierMask_char)
        return type_new(TypeKind_char);
    else if (type_specifiers == (TypeSpecifierMask_signed + TypeSpecifierMask_char))
        return type_new(TypeKind_schar);
    else if (type_specifiers == (TypeSpecifierMask_unsigned + TypeSpecifierMask_char))
        return type_new(TypeKind_uchar);
    else if (type_specifiers == TypeSpecifierMask_short ||
             type_specifiers == (TypeSpecifierMask_signed + TypeSpecifierMask_short) ||
             type_specifiers == (TypeSpecifierMask_short + TypeSpecifierMask_int) ||
             type_specifiers == (TypeSpecifierMask_signed + TypeSpecifierMask_short + TypeSpecifierMask_int))
        return type_new(TypeKind_short);
    else if (type_specifiers == (TypeSpecifierMask_unsigned + TypeSpecifierMask_short) ||
             type_specifiers == (TypeSpecifierMask_unsigned + TypeSpecifierMask_short + TypeSpecifierMask_int))
        return type_new(TypeKind_ushort);
    else if (type_specifiers == TypeSpecifierMask_int || type_specifiers == TypeSpecifierMask_signed ||
             type_specifiers == (TypeSpecifierMask_signed + TypeSpecifierMask_int))
        return type_new(TypeKind_int);
    else if (type_specifiers == TypeSpecifierMask_unsigned ||
             type_specifiers == (TypeSpecifierMask_unsigned + TypeSpecifierMask_int))
        return type_new(TypeKind_uint);
    else if (type_specifiers == TypeSpecifierMask_long ||
             type_specifiers == (TypeSpecifierMask_signed + TypeSpecifierMask_long) ||
             type_specifiers == (TypeSpecifierMask_long + TypeSpecifierMask_int) ||
             type_specifiers == (TypeSpecifierMask_signed + TypeSpecifierMask_long + TypeSpecifierMask_int))
        return type_new(TypeKind_long);
    else if (type_specifiers == (TypeSpecifierMask_unsigned + TypeSpecifierMask_long) ||
             type_specifiers == (TypeSpecifierMask_unsigned + TypeSpecifierMask_long + TypeSpecifierMask_int))
        return type_new(TypeKind_ulong);
    else if (type_specifiers == (TypeSpecifierMask_long + TypeSpecifierMask_long) ||
             type_specifiers == (TypeSpecifierMask_signed + TypeSpecifierMask_long + TypeSpecifierMask_long) ||
             type_specifiers == (TypeSpecifierMask_long + TypeSpecifierMask_long + TypeSpecifierMask_int) ||
             type_specifiers ==
                 (TypeSpecifierMask_signed + TypeSpecifierMask_long + TypeSpecifierMask_long + TypeSpecifierMask_int))
        return type_new(TypeKind_llong);
    else if (type_specifiers == (TypeSpecifierMask_unsigned + TypeSpecifierMask_long + TypeSpecifierMask_long) ||
             type_specifiers ==
                 (TypeSpecifierMask_unsigned + TypeSpecifierMask_long + TypeSpecifierMask_long + TypeSpecifierMask_int))
        return type_new(TypeKind_ullong);
    else if (type_specifiers == TypeSpecifierMask_float)
        return type_new(TypeKind_float);
    else if (type_specifiers == TypeSpecifierMask_double)
        return type_new(TypeKind_double);
    else if (type_specifiers == TypeSpecifierMask_long + TypeSpecifierMask_double)
        return type_new(TypeKind_ldouble);
    else if (type_specifiers == TypeSpecifierMask_bool)
        return type_new(TypeKind_bool);
    else if (type_specifiers == TypeSpecifierMask_struct)
        return NULL;
    else if (type_specifiers == TypeSpecifierMask_union)
        return NULL;
    else if (type_specifiers == TypeSpecifierMask_enum)
        return NULL;
    else if (type_specifiers == TypeSpecifierMask_typedef_name)
        return NULL;
    else if (type_specifiers == 0)
        return NULL;
    else
        unimplemented();
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
    int type_specifiers = 0;
    Type* ty = NULL;

    Token* tok;
    while (1) {
        tok = peek_token(p);

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
            if (type_specifiers & TypeSpecifierMask_void) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_void;
        } else if (tok->kind == TokenKind_keyword_char) {
            if (type_specifiers & TypeSpecifierMask_char) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_char;
        } else if (tok->kind == TokenKind_keyword_short) {
            if (type_specifiers & TypeSpecifierMask_short) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_short;
        } else if (tok->kind == TokenKind_keyword_int) {
            if (type_specifiers & TypeSpecifierMask_int) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_int;
        } else if (tok->kind == TokenKind_keyword_long) {
            if (type_specifiers & (TypeSpecifierMask_long + TypeSpecifierMask_long)) {
                fatal_error("%s:%d: too looong!", tok->loc.filename, tok->loc.line);
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_long;
        } else if (tok->kind == TokenKind_keyword_float) {
            if (type_specifiers & TypeSpecifierMask_float) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_float;
        } else if (tok->kind == TokenKind_keyword_double) {
            if (type_specifiers & TypeSpecifierMask_double) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_double;
        } else if (tok->kind == TokenKind_keyword_signed) {
            if (type_specifiers & TypeSpecifierMask_signed) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_signed;
        } else if (tok->kind == TokenKind_keyword_unsigned) {
            if (type_specifiers & TypeSpecifierMask_unsigned) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_unsigned;
        } else if (tok->kind == TokenKind_keyword__BitInt) {
            if (type_specifiers & TypeSpecifierMask__BitInt) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__BitInt;
        } else if (tok->kind == TokenKind_keyword_bool) {
            if (type_specifiers & TypeSpecifierMask_bool) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_bool;
        } else if (tok->kind == TokenKind_keyword__Complex) {
            if (type_specifiers & TypeSpecifierMask__Complex) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__Complex;
        } else if (tok->kind == TokenKind_keyword__Decimal32) {
            if (type_specifiers & TypeSpecifierMask__Decimal32) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__Decimal32;
        } else if (tok->kind == TokenKind_keyword__Decimal64) {
            if (type_specifiers & TypeSpecifierMask__Decimal64) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__Decimal64;
        } else if (tok->kind == TokenKind_keyword__Decimal128) {
            if (type_specifiers & TypeSpecifierMask__Decimal128) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__Decimal128;
        } else if (tok->kind == TokenKind_keyword__Atomic) {
            if (type_specifiers & TypeSpecifierMask__Atomic) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__Atomic;
        } else if (tok->kind == TokenKind_keyword_struct) {
            if (type_specifiers & TypeSpecifierMask_struct) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            ty = parse_struct_specifier(p);
            type_specifiers += TypeSpecifierMask_struct;
        } else if (tok->kind == TokenKind_keyword_union) {
            if (type_specifiers & TypeSpecifierMask_union) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            ty = parse_union_specifier(p);
            type_specifiers += TypeSpecifierMask_union;
        } else if (tok->kind == TokenKind_keyword_enum) {
            if (type_specifiers & TypeSpecifierMask_enum) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            ty = parse_enum_specifier(p);
            type_specifiers += TypeSpecifierMask_enum;
        } else if (tok->kind == TokenKind_keyword_typeof) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_typeof_unqual) {
            unimplemented();
        } else if (is_typedef_name(p, tok)) {
            if (type_specifiers & TypeSpecifierMask_typedef_name) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            int typedef_idx = find_typedef(p, tok->value.string);
            ty = p->typedefs->node_items[typedef_idx].ty;
            type_specifiers += TypeSpecifierMask_typedef_name;
        }
        // type-specifier-qualifier > type-qualifier
        else if (tok->kind == TokenKind_keyword_const) {
            // TODO
            next_token(p);
        } else if (tok->kind == TokenKind_keyword_restrict) {
            // TODO
            next_token(p);
        } else if (tok->kind == TokenKind_keyword_volatile) {
            // TODO
            next_token(p);
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

    Type* ty_ = distinguish_type_from_type_specifiers(type_specifiers);
    if (ty_) {
        ty = ty_;
    }
    if (!ty) {
        fatal_error("%s:%d: no type specifiers", tok->loc.filename, tok->loc.line);
    }

    ty->storage_class = storage_class;
    return ty;
}

// specifier-qualifier-list:
//     { type-specifier-qualifier }+ TODO attribute-specifier-sequence?
static Type* parse_specifier_qualifier_list(Parser* p) {
    int type_specifiers = 0;
    Type* ty = NULL;

    while (1) {
        Token* tok = peek_token(p);

        // type-specifier-qualifier > type-specifier
        if (tok->kind == TokenKind_keyword_void) {
            if (type_specifiers & TypeSpecifierMask_void) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_void;
        } else if (tok->kind == TokenKind_keyword_char) {
            if (type_specifiers & TypeSpecifierMask_char) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_char;
        } else if (tok->kind == TokenKind_keyword_short) {
            if (type_specifiers & TypeSpecifierMask_short) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_short;
        } else if (tok->kind == TokenKind_keyword_int) {
            if (type_specifiers & TypeSpecifierMask_int) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_int;
        } else if (tok->kind == TokenKind_keyword_long) {
            if (type_specifiers & (TypeSpecifierMask_long + TypeSpecifierMask_long)) {
                fatal_error("%s:%d: too looong!", tok->loc.filename, tok->loc.line);
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_long;
        } else if (tok->kind == TokenKind_keyword_float) {
            if (type_specifiers & TypeSpecifierMask_float) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_float;
        } else if (tok->kind == TokenKind_keyword_double) {
            if (type_specifiers & TypeSpecifierMask_double) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_double;
        } else if (tok->kind == TokenKind_keyword_signed) {
            if (type_specifiers & TypeSpecifierMask_signed) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_signed;
        } else if (tok->kind == TokenKind_keyword_unsigned) {
            if (type_specifiers & TypeSpecifierMask_unsigned) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_unsigned;
        } else if (tok->kind == TokenKind_keyword__BitInt) {
            if (type_specifiers & TypeSpecifierMask__BitInt) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__BitInt;
        } else if (tok->kind == TokenKind_keyword_bool) {
            if (type_specifiers & TypeSpecifierMask_bool) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask_bool;
        } else if (tok->kind == TokenKind_keyword__Complex) {
            if (type_specifiers & TypeSpecifierMask__Complex) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__Complex;
        } else if (tok->kind == TokenKind_keyword__Decimal32) {
            if (type_specifiers & TypeSpecifierMask__Decimal32) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__Decimal32;
        } else if (tok->kind == TokenKind_keyword__Decimal64) {
            if (type_specifiers & TypeSpecifierMask__Decimal64) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__Decimal64;
        } else if (tok->kind == TokenKind_keyword__Decimal128) {
            if (type_specifiers & TypeSpecifierMask__Decimal128) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__Decimal128;
        } else if (tok->kind == TokenKind_keyword__Atomic) {
            if (type_specifiers & TypeSpecifierMask__Atomic) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            type_specifiers += TypeSpecifierMask__Atomic;
        } else if (tok->kind == TokenKind_keyword_struct) {
            if (type_specifiers & TypeSpecifierMask_struct) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            ty = parse_struct_specifier(p);
            type_specifiers += TypeSpecifierMask_struct;
        } else if (tok->kind == TokenKind_keyword_union) {
            if (type_specifiers & TypeSpecifierMask_union) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            ty = parse_union_specifier(p);
            type_specifiers += TypeSpecifierMask_union;
        } else if (tok->kind == TokenKind_keyword_enum) {
            if (type_specifiers & TypeSpecifierMask_enum) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            ty = parse_enum_specifier(p);
            type_specifiers += TypeSpecifierMask_enum;
        } else if (tok->kind == TokenKind_keyword_typeof) {
            unimplemented();
        } else if (tok->kind == TokenKind_keyword_typeof_unqual) {
            unimplemented();
        } else if (is_typedef_name(p, tok)) {
            if (type_specifiers & TypeSpecifierMask_typedef_name) {
                fatal_error("%s:%d: duplicate '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
            }
            next_token(p);
            int typedef_idx = find_typedef(p, tok->value.string);
            ty = p->typedefs->node_items[typedef_idx].ty;
            type_specifiers += TypeSpecifierMask_typedef_name;
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

    Type* ty_ = distinguish_type_from_type_specifiers(type_specifiers);
    if (ty_) {
        ty = ty_;
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

    if (peek_token(p)->kind == TokenKind_brace_l) {
        return parse_func_def(p, decls);
    }

    if (ty->storage_class == StorageClass_typedef) {
        parse_typedef_decl(p, decls);
        return NULL;
    } else {
        expect(p, TokenKind_semicolon);
        declare_func_or_var(p, &decls->node_items[decls->node_len - 1]);
        return decls;
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
        } else if (n->kind == AstNodeKind_gvar_decl) {
            ast_append(vars, n);
        } else if (n->kind == AstNodeKind_list) {
            for (int i = 0; i < n->node_len; ++i) {
                if (n->node_items[i].kind == AstNodeKind_gvar_decl)
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
        } else if (e->node_op == TokenKind_and) {
            return v1 & v2;
        } else if (e->node_op == TokenKind_or) {
            return v1 | v2;
        } else if (e->node_op == TokenKind_xor) {
            return v1 ^ v2;
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
    } else if (e->kind == AstNodeKind_cast_expr) {
        return eval(e->node_operand);
    } else {
        unimplemented();
    }
}

bool pp_eval_constant_expression(TokenArray* pp_tokens) {
    TokenArray* tokens = convert_pp_tokens_to_tokens(pp_tokens);
    Parser* p = parser_new(tokens);
    AstNode* e = parse_constant_expression(p);
    return eval(e) != 0;
}
