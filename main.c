int atoi(const char*);
void* calloc(long, long);
void exit(int);
int getchar(void);
int isalnum(int);
int isalpha(int);
int isdigit(int);
int isspace(int);
void* memcpy(void*, void*, long);
int printf();
int sprintf();
int strcmp(const char*, const char*);
char* strstr(const char*, const char*);

#define NULL 0

void fatal_error(const char* msg) {
    printf("%s\n", msg);
    exit(1);
}

void unreachable() {
    fatal_error("unreachable");
}

char* read_all() {
    char* buf = calloc(1024 * 1024, sizeof(char));
    char* cur = buf;
    while (1) {
        int c = getchar();
        if (c == -1) {
            break;
        }
        *cur = c;
        ++cur;
    }
    return buf;
}

enum TokenKind {
    TokenKind_eof,

    TokenKind_and,
    TokenKind_andand,
    TokenKind_arrow,
    TokenKind_assign,
    TokenKind_assign_add,
    TokenKind_assign_sub,
    TokenKind_brace_l,
    TokenKind_brace_r,
    TokenKind_bracket_l,
    TokenKind_bracket_r,
    TokenKind_comma,
    TokenKind_dot,
    TokenKind_eq,
    TokenKind_ge,
    TokenKind_gt,
    TokenKind_ident,
    TokenKind_keyword_break,
    TokenKind_keyword_char,
    TokenKind_keyword_const,
    TokenKind_keyword_continue,
    TokenKind_keyword_else,
    TokenKind_keyword_enum,
    TokenKind_keyword_for,
    TokenKind_keyword_if,
    TokenKind_keyword_int,
    TokenKind_keyword_long,
    TokenKind_keyword_return,
    TokenKind_keyword_sizeof,
    TokenKind_keyword_struct,
    TokenKind_keyword_void,
    TokenKind_keyword_while,
    TokenKind_le,
    TokenKind_lt,
    TokenKind_literal_int,
    TokenKind_literal_str,
    TokenKind_minus,
    TokenKind_minusminus,
    TokenKind_ne,
    TokenKind_not,
    TokenKind_oror,
    TokenKind_paren_l,
    TokenKind_paren_r,
    TokenKind_percent,
    TokenKind_plus,
    TokenKind_plusplus,
    TokenKind_semicolon,
    TokenKind_slash,
    TokenKind_star,
};

struct Token {
    enum TokenKind kind;
    char* value;
};

struct Define {
    char* from;
    struct Token* to;
};

struct Token* tokenize(char* src) {
    struct Token* tokens = calloc(1024 * 1024, sizeof(struct Token));
    struct Token* tok = tokens;
    struct Define* defines = calloc(1024, sizeof(struct Define));
    struct Define* def = defines;
    int pos = 0;
    int ch;
    int start;
    while (src[pos]) {
        char c = src[pos];
        ++pos;
        if (c == '(') {
            tok->kind = TokenKind_paren_l;
        } else if (c == ')') {
            tok->kind = TokenKind_paren_r;
        } else if (c == '{') {
            tok->kind = TokenKind_brace_l;
        } else if (c == '}') {
            tok->kind = TokenKind_brace_r;
        } else if (c == '[') {
            tok->kind = TokenKind_bracket_l;
        } else if (c == ']') {
            tok->kind = TokenKind_bracket_r;
        } else if (c == ',') {
            tok->kind = TokenKind_comma;
        } else if (c == ';') {
            tok->kind = TokenKind_semicolon;
        } else if (c == '+') {
            if (src[pos] == '=') {
                ++pos;
                tok->kind = TokenKind_assign_add;
            } else if (src[pos] == '+') {
                ++pos;
                tok->kind = TokenKind_plusplus;
            } else {
                tok->kind = TokenKind_plus;
            }
        } else if (c == '|') {
            ++pos;
            tok->kind = TokenKind_oror;
        } else if (c == '&') {
            if (src[pos] == '&') {
                ++pos;
                tok->kind = TokenKind_andand;
            } else {
                tok->kind = TokenKind_and;
            }
        } else if (c == '-') {
            if (src[pos] == '>') {
                ++pos;
                tok->kind = TokenKind_arrow;
            } else if (src[pos] == '=') {
                ++pos;
                tok->kind = TokenKind_assign_sub;
            } else if (src[pos] == '-') {
                ++pos;
                tok->kind = TokenKind_minusminus;
            } else {
                tok->kind = TokenKind_minus;
            }
        } else if (c == '*') {
            tok->kind = TokenKind_star;
        } else if (c == '/') {
            tok->kind = TokenKind_slash;
        } else if (c == '%') {
            tok->kind = TokenKind_percent;
        } else if (c == '.') {
            tok->kind = TokenKind_dot;
        } else if (c == '!') {
            if (src[pos] == '=') {
                ++pos;
                tok->kind = TokenKind_ne;
            } else {
                tok->kind = TokenKind_not;
            }
        } else if (c == '=') {
            if (src[pos] == '=') {
                ++pos;
                tok->kind = TokenKind_eq;
            } else {
                tok->kind = TokenKind_assign;
            }
        } else if (c == '<') {
            if (src[pos] == '=') {
                ++pos;
                tok->kind = TokenKind_le;
            } else {
                tok->kind = TokenKind_lt;
            }
        } else if (c == '>') {
            if (src[pos] == '=') {
                ++pos;
                tok->kind = TokenKind_ge;
            } else {
                tok->kind = TokenKind_gt;
            }
        } else if (c == '\'') {
            ch = src[pos];
            if (ch == '\\') {
                ++pos;
                ch = src[pos];
                if (ch == 'n') {
                    ch = '\n';
                }
            }
            pos += 2;
            tok->kind = TokenKind_literal_int;
            tok->value = calloc(4, sizeof(char));
            sprintf(tok->value, "%d", ch);
        } else if (c == '"') {
            start = pos;
            while (1) {
                ch = src[pos];
                if (ch == '\\') {
                    ++pos;
                } else if (ch == '"') {
                    break;
                }
                ++pos;
            }
            tok->kind = TokenKind_literal_str;
            tok->value = calloc(pos - start + 1, sizeof(char));
            memcpy(tok->value, src + start, pos - start);
            ++pos;
        } else if (isdigit(c)) {
            --pos;
            start = pos;
            while (isdigit(src[pos])) {
                ++pos;
            }
            tok->kind = TokenKind_literal_int;
            tok->value = calloc(pos - start + 1, sizeof(char));
            memcpy(tok->value, src + start, pos - start);
        } else if (isalpha(c) || c == '_') {
            --pos;
            start = pos;
            while (isalnum(src[pos]) || src[pos] == '_') {
                ++pos;
            }
            int ident_len = pos - start;
            if (ident_len == 5 && strstr(src + start, "break") == src + start) {
                tok->kind = TokenKind_keyword_break;
            } else if (ident_len == 4 && strstr(src + start, "char") == src + start) {
                tok->kind = TokenKind_keyword_char;
            } else if (ident_len == 5 && strstr(src + start, "const") == src + start) {
                tok->kind = TokenKind_keyword_const;
            } else if (ident_len == 8 && strstr(src + start, "continue") == src + start) {
                tok->kind = TokenKind_keyword_continue;
            } else if (ident_len == 4 && strstr(src + start, "else") == src + start) {
                tok->kind = TokenKind_keyword_else;
            } else if (ident_len == 4 && strstr(src + start, "enum") == src + start) {
                tok->kind = TokenKind_keyword_enum;
            } else if (ident_len == 3 && strstr(src + start, "for") == src + start) {
                tok->kind = TokenKind_keyword_for;
            } else if (ident_len == 2 && strstr(src + start, "if") == src + start) {
                tok->kind = TokenKind_keyword_if;
            } else if (ident_len == 3 && strstr(src + start, "int") == src + start) {
                tok->kind = TokenKind_keyword_int;
            } else if (ident_len == 4 && strstr(src + start, "long") == src + start) {
                tok->kind = TokenKind_keyword_long;
            } else if (ident_len == 6 && strstr(src + start, "return") == src + start) {
                tok->kind = TokenKind_keyword_return;
            } else if (ident_len == 6 && strstr(src + start, "sizeof") == src + start) {
                tok->kind = TokenKind_keyword_sizeof;
            } else if (ident_len == 6 && strstr(src + start, "struct") == src + start) {
                tok->kind = TokenKind_keyword_struct;
            } else if (ident_len == 4 && strstr(src + start, "void") == src + start) {
                tok->kind = TokenKind_keyword_void;
            } else if (ident_len == 5 && strstr(src + start, "while") == src + start) {
                tok->kind = TokenKind_keyword_while;
            } else {
                tok->value = calloc(ident_len + 1, sizeof(char));
                memcpy(tok->value, src + start, ident_len);
                int i = 0;
                while (defines + i != def) {
                    if (strcmp(tok->value, defines[i].from) == 0) {
                        tok->kind = defines[i].to->kind;
                        tok->value = defines[i].to->value;
                        break;
                    }
                    ++i;
                }
                if (defines + i == def) {
                    tok->kind = TokenKind_ident;
                }
            }
        } else if (isspace(c)) {
            continue;
        } else if (c == '#') {
            pos += 6;
            while (isspace(src[pos])) {
                ++pos;
            }
            start = pos;
            while (isalnum(src[pos]) || src[pos] == '_') {
                ++pos;
            }
            def->from = calloc(pos - start + 1, sizeof(char));
            memcpy(def->from, src + start, pos - start);
            while (isspace(src[pos])) {
                ++pos;
            }
            int start2 = pos;
            int is_digit = isdigit(src[pos]);
            if (is_digit) {
                while (isdigit(src[pos])) {
                    ++pos;
                }
            } else {
                while (isalnum(src[pos]) || src[pos] == '_') {
                    ++pos;
                }
            }
            def->to = calloc(1, sizeof(struct Token));
            if (is_digit) {
                def->to->kind = TokenKind_literal_int;
            } else {
                def->to->kind = TokenKind_ident;
            }
            def->to->value = calloc(pos - start2 + 1, sizeof(char));
            memcpy(def->to->value, src + start2, pos - start2);
            ++def;
            continue;
        } else {
            char* buf = calloc(1024, sizeof(char));
            sprintf(buf, "unknown token char(%d)", c);
            fatal_error(buf);
        }
        ++tok;
    }
    return tokens;
}

enum TypeKind {
    TypeKind_unknown,

    TypeKind_char,
    TypeKind_int,
    TypeKind_long,
    TypeKind_void,
    TypeKind_ptr,
    TypeKind_enum,
    TypeKind_struct,
};

struct AstNode;

struct Type {
    enum TypeKind kind;
    struct Type* to;
    struct AstNode* def;
};

struct Type* type_new(enum TypeKind kind) {
    struct Type* ty = calloc(1, sizeof(struct Type));
    ty->kind = kind;
    return ty;
}

struct Type* type_new_ptr(struct Type* to) {
    struct Type* ty = calloc(1, sizeof(struct Type));
    ty->kind = TypeKind_ptr;
    ty->to = to;
    return ty;
}

int type_is_unsized(struct Type* ty) {
    return ty->kind != TypeKind_void;
}

int type_sizeof_struct(struct Type* ty);
int type_alignof_struct(struct Type* ty);
int type_offsetof(struct Type* ty, const char* name);
struct Type* type_member_typeof(struct Type* ty, const char* name);

int type_sizeof(struct Type* ty) {
    if (!type_is_unsized(ty)) {
        fatal_error("type_sizeof: type size cannot be determined");
    }

    if (ty->kind == TypeKind_ptr) {
        return 8;
    } else if (ty->kind == TypeKind_char) {
        return 1;
    } else if (ty->kind == TypeKind_int) {
        return 4;
    } else if (ty->kind == TypeKind_long) {
        return 8;
    } else if (ty->kind == TypeKind_enum) {
        return 4;
    } else {
        return type_sizeof_struct(ty);
    }
}

int type_alignof(struct Type* ty) {
    if (!type_is_unsized(ty)) {
        fatal_error("type_alignof: type size cannot be determined");
    }

    if (ty->kind == TypeKind_ptr) {
        return 8;
    } else if (ty->kind == TypeKind_char) {
        return 1;
    } else if (ty->kind == TypeKind_int) {
        return 4;
    } else if (ty->kind == TypeKind_long) {
        return 8;
    } else if (ty->kind == TypeKind_enum) {
        return 4;
    } else {
        return type_alignof_struct(ty);
    }
}

enum AstNodeKind {
    AstNodeKind_unknown,

    AstNodeKind_assign_expr,
    AstNodeKind_binary_expr,
    AstNodeKind_break_stmt,
    AstNodeKind_continue_stmt,
    AstNodeKind_deref_expr,
    AstNodeKind_enum_def,
    AstNodeKind_enum_member,
    AstNodeKind_expr_stmt,
    AstNodeKind_for_stmt,
    AstNodeKind_func_call,
    AstNodeKind_func_decl,
    AstNodeKind_func_def,
    AstNodeKind_if_stmt,
    AstNodeKind_int_expr,
    AstNodeKind_list,
    AstNodeKind_logical_expr,
    AstNodeKind_lvar,
    AstNodeKind_param,
    AstNodeKind_ref_expr,
    AstNodeKind_return_stmt,
    AstNodeKind_struct_decl,
    AstNodeKind_struct_def,
    AstNodeKind_struct_member,
    AstNodeKind_str_expr,
    AstNodeKind_type,
    AstNodeKind_unary_expr,
    AstNodeKind_var_decl,
};

#define node_items __n1
#define node_len __i
#define node_expr __n1
#define node_lhs __n1
#define node_rhs __n2
#define node_operand __n1
#define node_cond __n1
#define node_init __n2
#define node_update __n3
#define node_then __n2
#define node_else __n3
#define node_body __n4
#define node_members __n1
#define node_params __n1
#define node_args __n1
#define node_int_value __i
#define node_idx __i
#define node_op __i

struct AstNode {
    enum AstNodeKind kind;
    char* name;
    struct Type* ty;
    struct AstNode* __n1;
    struct AstNode* __n2;
    struct AstNode* __n3;
    struct AstNode* __n4;
    int __i;
};

struct Program {
    struct AstNode* funcs;
    char** str_literals;
};

struct AstNode* ast_new(enum AstNodeKind kind) {
    struct AstNode* ast = calloc(1, sizeof(struct AstNode));
    ast->kind = kind;
    return ast;
}

struct AstNode* ast_new_list(int capacity) {
    struct AstNode* list = ast_new(AstNodeKind_list);
    list->node_items = calloc(capacity, sizeof(struct AstNode));
    list->node_len = 0;
    return list;
}

void ast_append(struct AstNode* list, struct AstNode* item) {
    if (list->kind != AstNodeKind_list) {
        fatal_error("ast_append: ast is not a list");
    }
    if (!item) {
        return;
    }
    memcpy(list->node_items + list->node_len, item, sizeof(struct AstNode));
    ++list->node_len;
}

struct AstNode* ast_new_int(int v) {
    struct AstNode* e = ast_new(AstNodeKind_int_expr);
    e->node_int_value = v;
    e->ty = type_new(TypeKind_int);
    return e;
}

struct AstNode* ast_new_unary_expr(int op, struct AstNode* operand) {
    struct AstNode* e = ast_new(AstNodeKind_unary_expr);
    e->node_op = op;
    e->node_operand = operand;
    e->ty = type_new(TypeKind_int);
    return e;
}

struct AstNode* ast_new_binary_expr(int op, struct AstNode* lhs, struct AstNode* rhs) {
    struct AstNode* e = ast_new(AstNodeKind_binary_expr);
    e->node_op = op;
    e->node_lhs = lhs;
    e->node_rhs = rhs;
    if (op == TokenKind_plus) {
        if (lhs->ty->kind == TypeKind_ptr) {
            e->ty = lhs->ty;
        } else if (rhs->ty->kind == TypeKind_ptr) {
            e->ty = rhs->ty;
        } else {
            e->ty = type_new(TypeKind_int);
        }
    } else if (op == TokenKind_minus) {
        if (lhs->ty->kind == TypeKind_ptr) {
            e->ty = lhs->ty;
        } else {
            e->ty = type_new(TypeKind_int);
        }
    } else {
        e->ty = type_new(TypeKind_int);
    }
    return e;
}

struct AstNode* ast_new_assign_expr(int op, struct AstNode* lhs, struct AstNode* rhs) {
    struct AstNode* e = ast_new(AstNodeKind_assign_expr);
    e->node_op = op;
    e->node_lhs = lhs;
    e->node_rhs = rhs;
    e->ty = lhs->ty;
    return e;
}

struct AstNode* ast_new_assign_add_expr(struct AstNode* lhs, struct AstNode* rhs) {
    if (lhs->ty->kind == TypeKind_ptr) {
        rhs = ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->to)));
    } else if (rhs->ty->kind == TypeKind_ptr) {
        lhs = ast_new_binary_expr(TokenKind_star, lhs, ast_new_int(type_sizeof(rhs->ty->to)));
    }
    return ast_new_assign_expr(TokenKind_assign_add, lhs, rhs);
}

struct AstNode* ast_new_assign_sub_expr(struct AstNode* lhs, struct AstNode* rhs) {
    if (lhs->ty->kind == TypeKind_ptr) {
        rhs = ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->to)));
    }
    return ast_new_assign_expr(TokenKind_assign_sub, lhs, rhs);
}

struct AstNode* ast_new_ref_expr(struct AstNode* operand) {
    struct AstNode* e = ast_new(AstNodeKind_ref_expr);
    e->node_operand = operand;
    e->ty = type_new_ptr(operand->ty);
    return e;
}

struct AstNode* ast_new_deref_expr(struct AstNode* operand) {
    struct AstNode* e = ast_new(AstNodeKind_deref_expr);
    e->node_operand = operand;
    e->ty = operand->ty->to;
    return e;
}

struct AstNode* ast_new_member_access_expr(struct AstNode* obj, char* name) {
    struct AstNode* e = ast_new(AstNodeKind_deref_expr);
    e->node_operand = ast_new_binary_expr(TokenKind_plus, obj, ast_new_int(type_offsetof(obj->ty->to, name)));
    e->ty = type_member_typeof(obj->ty->to, name);
    return e;
}

int type_sizeof_struct(struct Type* ty) {
    int next_offset = 0;
    int struct_align = 0;
    int padding;

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        struct AstNode* member = ty->def->node_members->node_items + i;
        int size = type_sizeof(member->ty);
        int align = type_alignof(member->ty);

        if (next_offset % align != 0) {
            padding = align - next_offset % align;
            next_offset += padding;
        }
        next_offset += size;
        if (struct_align < align) {
            struct_align = align;
        }
    }
    if (next_offset % struct_align != 0) {
        padding = struct_align - next_offset % struct_align;
        next_offset += padding;
    }
    return next_offset;
}

int type_alignof_struct(struct Type* ty) {
    int struct_align = 0;

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        struct AstNode* member = ty->def->node_members->node_items + i;
        int align = type_alignof(member->ty);

        if (struct_align < align) {
            struct_align = align;
        }
    }
    return struct_align;
}

int type_offsetof(struct Type* ty, const char* name) {
    if (ty->kind != TypeKind_struct) {
        fatal_error("type_offsetof: type is not a struct");
    }

    int next_offset = 0;

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        struct AstNode* member = ty->def->node_members->node_items + i;
        int size = type_sizeof(member->ty);
        int align = type_alignof(member->ty);

        if (next_offset % align != 0) {
            int padding = align - next_offset % align;
            next_offset += padding;
        }
        if (strcmp(member->name, name) == 0) {
            return next_offset;
        }
        next_offset += size;
    }

    fatal_error("type_offsetof: member not found");
}

struct Type* type_member_typeof(struct Type* ty, const char* name) {
    if (ty->kind != TypeKind_struct) {
        fatal_error("type_offsetof: type is not a struct");
    }

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        struct AstNode* member = ty->def->node_members->node_items + i;
        if (strcmp(member->name, name) == 0) {
            return member->ty;
        }
    }

    fatal_error("type_offsetof: member not found");
}

#define LVAR_MAX 32

struct LVar {
    char* name;
    struct Type* ty;
};

struct Func {
    char* name;
    struct Type* ty;
};

struct Parser {
    struct Token* tokens;
    int pos;
    struct LVar* lvars;
    int n_lvars;
    struct Func* funcs;
    int n_funcs;
    struct AstNode* structs;
    int n_structs;
    struct AstNode* enums;
    int n_enums;
    char** str_literals;
    int n_str_literals;
};

struct Parser* parser_new(struct Token* tokens) {
    struct Parser* p = calloc(1, sizeof(struct Parser));
    p->tokens = tokens;
    p->funcs = calloc(128, sizeof(struct Func));
    p->structs = calloc(64, sizeof(struct AstNode));
    p->enums = calloc(16, sizeof(struct AstNode));
    p->str_literals = calloc(1024, sizeof(char*));
    return p;
}

struct Token* peek_token(struct Parser* p) {
    return p->tokens + p->pos;
}

struct Token* next_token(struct Parser* p) {
    ++p->pos;
    return p->tokens + p->pos - 1;
}

int eof(struct Parser* p) {
    return peek_token(p)->kind != TokenKind_eof;
}

struct Token* expect(struct Parser* p, int expected) {
    struct Token* t = next_token(p);
    if (t->kind == expected) {
        return t;
    }

    char* buf = calloc(1024, sizeof(char));
    sprintf(buf, "expected %d, but got %d", expected, t->kind);
    fatal_error(buf);
}

int find_lvar(struct Parser* p, const char* name) {
    int i;
    for (i = 0; i < p->n_lvars; ++i) {
        if (strcmp(p->lvars[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_func(struct Parser* p, const char* name) {
    int i;
    for (i = 0; i < p->n_funcs; ++i) {
        if (strcmp(p->funcs[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_struct(struct Parser* p, const char* name) {
    int i;
    for (i = 0; i < p->n_structs; ++i) {
        if (strcmp(p->structs[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_enum(struct Parser* p, const char* name) {
    int i;
    for (i = 0; i < p->n_enums; ++i) {
        if (strcmp(p->enums[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_enum_member(struct Parser* p, const char* name) {
    int i;
    int j;
    for (i = 0; i < p->n_enums; ++i) {
        for (j = 0; j < p->enums[i].node_members->node_len; ++j) {
            if (strcmp(p->enums[i].node_members->node_items[j].name, name) == 0) {
                return i * 1000 + j;
            }
        }
    }
    return -1;
}

struct AstNode* parse_expr(struct Parser* p);
struct AstNode* parse_stmt(struct Parser* p);

char* parse_ident(struct Parser* p) {
    return expect(p, TokenKind_ident)->value;
}

int register_str_literal(struct Parser* p, char* s) {
    p->str_literals[p->n_str_literals] = s;
    ++p->n_str_literals;
    return p->n_str_literals;
}

struct AstNode* parse_primary_expr(struct Parser* p) {
    struct Token* t = next_token(p);
    struct AstNode* e;
    char* buf;
    if (t->kind == TokenKind_literal_int) {
        return ast_new_int(atoi(t->value));
    } else if (t->kind == TokenKind_literal_str) {
        e = ast_new(AstNodeKind_str_expr);
        e->node_idx = register_str_literal(p, t->value);
        return e;
    } else if (t->kind == TokenKind_paren_l) {
        e = parse_expr(p);
        expect(p, TokenKind_paren_r);
        return e;
    } else if (t->kind == TokenKind_ident) {
        char* name = t->value;

        if (peek_token(p)->kind == TokenKind_paren_l) {
            e = ast_new(AstNodeKind_func_call);
            int func_idx = find_func(p, name);
            if (func_idx == -1) {
                buf = calloc(1024, sizeof(char));
                sprintf(buf, "undefined function: %s", name);
                fatal_error(buf);
            }
            e->name = name;
            e->ty = p->funcs[func_idx].ty;
            return e;
        }

        int var_idx = find_lvar(p, name);
        if (var_idx == -1) {
            int enum_member_idx = find_enum_member(p, name);
            if (enum_member_idx == -1) {
                buf = calloc(1024, sizeof(char));
                sprintf(buf, "undefined variable: %s", name);
                fatal_error(buf);
            }
            int enum_idx = enum_member_idx / 1000;
            int n = enum_member_idx % 1000;
            e = ast_new_int(p->enums[enum_idx].node_members->node_items[n].node_int_value);
            e->ty = type_new(TypeKind_enum);
            e->ty->def = p->enums + enum_idx;
            return e;
        }

        e = ast_new(AstNodeKind_lvar);
        e->name = name;
        e->node_idx = var_idx;
        e->ty = p->lvars[var_idx].ty;
        return e;
    } else {
        buf = calloc(1024, sizeof(char));
        sprintf(buf, "expected primary expression, but got %d", t->kind);
        fatal_error(buf);
    }
}

struct AstNode* parse_arg_list(struct Parser* p) {
    struct AstNode* list = ast_new_list(6);
    while (peek_token(p)->kind != TokenKind_paren_r) {
        struct AstNode* arg = parse_expr(p);
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

struct AstNode* parse_postfix_expr(struct Parser* p) {
    struct AstNode* ret = parse_primary_expr(p);
    struct AstNode* e;
    char* name;
    while (1) {
        enum TokenKind tk = peek_token(p)->kind;
        if (tk == TokenKind_paren_l) {
            next_token(p);
            struct AstNode* args = parse_arg_list(p);
            expect(p, TokenKind_paren_r);
            ret->node_args = args;
        } else if (tk == TokenKind_bracket_l) {
            next_token(p);
            struct AstNode* idx = parse_expr(p);
            expect(p, TokenKind_bracket_r);
            idx = ast_new_binary_expr(TokenKind_star, idx, ast_new_int(type_sizeof(ret->ty->to)));
            ret = ast_new_deref_expr(ast_new_binary_expr(TokenKind_plus, ret, idx));
        } else if (tk == TokenKind_dot) {
            next_token(p);
            name = parse_ident(p);
            ret = ast_new_member_access_expr(ast_new_ref_expr(ret), name);
        } else if (tk == TokenKind_arrow) {
            next_token(p);
            name = parse_ident(p);
            ret = ast_new_member_access_expr(ret, name);
        } else {
            break;
        }
    }
    return ret;
}

int is_type_token(enum TokenKind token_kind) {
    return token_kind == TokenKind_keyword_int || token_kind == TokenKind_keyword_long ||
           token_kind == TokenKind_keyword_char || token_kind == TokenKind_keyword_void ||
           token_kind == TokenKind_keyword_enum || token_kind == TokenKind_keyword_struct ||
           token_kind == TokenKind_keyword_const;
}

struct Type* parse_type(struct Parser* p) {
    struct Token* t = next_token(p);
    char* buf;
    char* name;
    if (!is_type_token(t->kind)) {
        buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_type: unknown type, %d", t->kind);
        fatal_error(buf);
    }
    if (t->kind == TokenKind_keyword_const) {
        t = next_token(p);
    }
    struct Type* ty = type_new(TypeKind_unknown);
    if (t->kind == TokenKind_keyword_int) {
        ty->kind = TypeKind_int;
    } else if (t->kind == TokenKind_keyword_long) {
        ty->kind = TypeKind_long;
    } else if (t->kind == TokenKind_keyword_char) {
        ty->kind = TypeKind_char;
    } else if (t->kind == TokenKind_keyword_void) {
        ty->kind = TypeKind_void;
    } else if (t->kind == TokenKind_keyword_enum) {
        ty->kind = TypeKind_enum;
        name = parse_ident(p);
        int enum_idx = find_enum(p, name);
        if (enum_idx == -1) {
            buf = calloc(1024, sizeof(char));
            sprintf(buf, "parse_type: unknown enum, %s", name);
            fatal_error(buf);
        }
        ty->def = p->enums + enum_idx;
    } else if (t->kind == TokenKind_keyword_struct) {
        ty->kind = TypeKind_struct;
        name = parse_ident(p);
        int struct_idx = find_struct(p, name);
        if (struct_idx == -1) {
            buf = calloc(1024, sizeof(char));
            sprintf(buf, "parse_type: unknown struct, %s", name);
            fatal_error(buf);
        }
        ty->def = p->structs + struct_idx;
    } else {
        unreachable();
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

struct AstNode* parse_prefix_expr(struct Parser* p) {
    struct AstNode* operand;
    enum TokenKind op = peek_token(p)->kind;
    if (op == TokenKind_minus) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_binary_expr(op, ast_new_int(0), operand);
    } else if (op == TokenKind_not) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_unary_expr(op, operand);
    } else if (op == TokenKind_and) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_ref_expr(operand);
    } else if (op == TokenKind_star) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_deref_expr(operand);
    } else if (op == TokenKind_plusplus) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_assign_add_expr(operand, ast_new_int(1));
    } else if (op == TokenKind_minusminus) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_assign_sub_expr(operand, ast_new_int(1));
    } else if (op == TokenKind_keyword_sizeof) {
        next_token(p);
        expect(p, TokenKind_paren_l);
        struct Type* ty = parse_type(p);
        expect(p, TokenKind_paren_r);
        return ast_new_int(type_sizeof(ty));
    }
    return parse_postfix_expr(p);
}

struct AstNode* parse_multiplicative_expr(struct Parser* p) {
    struct AstNode* lhs = parse_prefix_expr(p);
    while (1) {
        enum TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_star || op == TokenKind_slash || op == TokenKind_percent) {
            next_token(p);
            struct AstNode* rhs = parse_prefix_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

struct AstNode* parse_additive_expr(struct Parser* p) {
    struct AstNode* lhs = parse_multiplicative_expr(p);
    struct AstNode* rhs;
    while (1) {
        enum TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_plus) {
            next_token(p);
            rhs = parse_multiplicative_expr(p);
            if (lhs->ty->kind == TypeKind_ptr) {
                lhs = ast_new_binary_expr(
                    op, lhs, ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->to))));
            } else if (rhs->ty->kind == TypeKind_ptr) {
                lhs = ast_new_binary_expr(
                    op, ast_new_binary_expr(TokenKind_star, lhs, ast_new_int(type_sizeof(rhs->ty->to))), rhs);
            } else {
                lhs = ast_new_binary_expr(op, lhs, rhs);
            }
        } else if (op == TokenKind_minus) {
            next_token(p);
            rhs = parse_multiplicative_expr(p);
            if (lhs->ty->kind == TypeKind_ptr) {
                lhs = ast_new_binary_expr(
                    op, lhs, ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->to))));
            } else {
                lhs = ast_new_binary_expr(op, lhs, rhs);
            }
        } else {
            break;
        }
    }
    return lhs;
}

struct AstNode* parse_relational_expr(struct Parser* p) {
    struct AstNode* lhs = parse_additive_expr(p);
    struct AstNode* rhs;
    while (1) {
        enum TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_lt || op == TokenKind_le) {
            next_token(p);
            rhs = parse_additive_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
        } else if (op == TokenKind_gt) {
            next_token(p);
            rhs = parse_additive_expr(p);
            lhs = ast_new_binary_expr(TokenKind_lt, rhs, lhs);
        } else if (op == TokenKind_ge) {
            next_token(p);
            rhs = parse_additive_expr(p);
            lhs = ast_new_binary_expr(TokenKind_le, rhs, lhs);
        } else {
            break;
        }
    }
    return lhs;
}

struct AstNode* parse_equality_expr(struct Parser* p) {
    struct AstNode* lhs = parse_relational_expr(p);
    while (1) {
        enum TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_eq || op == TokenKind_ne) {
            next_token(p);
            struct AstNode* rhs = parse_relational_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

struct AstNode* parse_logical_and_expr(struct Parser* p) {
    struct AstNode* lhs = parse_equality_expr(p);
    while (1) {
        enum TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_andand) {
            next_token(p);
            struct AstNode* rhs = parse_equality_expr(p);
            struct AstNode* e = ast_new(AstNodeKind_logical_expr);
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

struct AstNode* parse_logical_or_expr(struct Parser* p) {
    struct AstNode* lhs = parse_logical_and_expr(p);
    while (1) {
        enum TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_oror) {
            next_token(p);
            struct AstNode* rhs = parse_logical_and_expr(p);
            struct AstNode* e = ast_new(AstNodeKind_logical_expr);
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

struct AstNode* parse_assignment_expr(struct Parser* p) {
    struct AstNode* lhs = parse_logical_or_expr(p);
    struct AstNode* rhs;
    while (1) {
        enum TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_assign) {
            next_token(p);
            rhs = parse_logical_or_expr(p);
            lhs = ast_new_assign_expr(op, lhs, rhs);
        } else if (op == TokenKind_assign_add) {
            next_token(p);
            rhs = parse_logical_or_expr(p);
            lhs = ast_new_assign_add_expr(lhs, rhs);
        } else if (op == TokenKind_assign_sub) {
            next_token(p);
            rhs = parse_logical_or_expr(p);
            lhs = ast_new_assign_sub_expr(lhs, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

struct AstNode* parse_expr(struct Parser* p) {
    return parse_assignment_expr(p);
}

struct AstNode* parse_return_stmt(struct Parser* p) {
    expect(p, TokenKind_keyword_return);
    if (peek_token(p)->kind == TokenKind_semicolon) {
        next_token(p);
        return ast_new(AstNodeKind_return_stmt);
    }

    struct AstNode* expr = parse_expr(p);
    expect(p, TokenKind_semicolon);

    struct AstNode* ret = ast_new(AstNodeKind_return_stmt);
    ret->node_expr = expr;
    return ret;
}

struct AstNode* parse_if_stmt(struct Parser* p) {
    expect(p, TokenKind_keyword_if);
    expect(p, TokenKind_paren_l);
    struct AstNode* cond = parse_expr(p);
    expect(p, TokenKind_paren_r);
    struct AstNode* then_body = parse_stmt(p);
    struct AstNode* else_body = NULL;
    if (peek_token(p)->kind == TokenKind_keyword_else) {
        next_token(p);
        else_body = parse_stmt(p);
    }

    struct AstNode* stmt = ast_new(AstNodeKind_if_stmt);
    stmt->node_cond = cond;
    stmt->node_then = then_body;
    stmt->node_else = else_body;
    return stmt;
}

struct AstNode* parse_for_stmt(struct Parser* p) {
    expect(p, TokenKind_keyword_for);
    expect(p, TokenKind_paren_l);
    struct AstNode* init = NULL;
    struct AstNode* cond = NULL;
    struct AstNode* update = NULL;
    if (peek_token(p)->kind != TokenKind_semicolon) {
        init = parse_expr(p);
    }
    expect(p, TokenKind_semicolon);
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
    struct AstNode* body = parse_stmt(p);

    struct AstNode* stmt = ast_new(AstNodeKind_for_stmt);
    stmt->node_cond = cond;
    stmt->node_init = init;
    stmt->node_update = update;
    stmt->node_body = body;
    return stmt;
}

struct AstNode* parse_while_stmt(struct Parser* p) {
    expect(p, TokenKind_keyword_while);
    expect(p, TokenKind_paren_l);
    struct AstNode* cond = parse_expr(p);
    expect(p, TokenKind_paren_r);
    struct AstNode* body = parse_stmt(p);

    struct AstNode* stmt = ast_new(AstNodeKind_for_stmt);
    stmt->node_cond = cond;
    stmt->node_body = body;
    return stmt;
}

struct AstNode* parse_break_stmt(struct Parser* p) {
    expect(p, TokenKind_keyword_break);
    expect(p, TokenKind_semicolon);
    return ast_new(AstNodeKind_break_stmt);
}

struct AstNode* parse_continue_stmt(struct Parser* p) {
    expect(p, TokenKind_keyword_continue);
    expect(p, TokenKind_semicolon);
    return ast_new(AstNodeKind_continue_stmt);
}

struct AstNode* parse_var_decl(struct Parser* p) {
    struct Type* ty = parse_type(p);
    if (!type_is_unsized(ty)) {
        fatal_error("parse_var_decl: invalid type for variable");
    }
    char* name = parse_ident(p);

    struct AstNode* init = NULL;
    if (peek_token(p)->kind == TokenKind_assign) {
        next_token(p);
        init = parse_expr(p);
    }
    expect(p, TokenKind_semicolon);

    if (find_lvar(p, name) != -1) {
        char* buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_var_decl: %s redeclared", name);
        fatal_error(buf);
    }
    p->lvars[p->n_lvars].name = name;
    p->lvars[p->n_lvars].ty = ty;
    ++p->n_lvars;

    struct AstNode* ret;
    if (init) {
        struct AstNode* lhs = ast_new(AstNodeKind_lvar);
        lhs->name = name;
        lhs->node_idx = p->n_lvars - 1;
        lhs->ty = ty;
        struct AstNode* assign = ast_new_assign_expr(TokenKind_assign, lhs, init);
        ret = ast_new(AstNodeKind_expr_stmt);
        ret->node_expr = assign;
    } else {
        ret = ast_new(AstNodeKind_var_decl);
    }
    return ret;
}

struct AstNode* parse_expr_stmt(struct Parser* p) {
    struct AstNode* e = parse_expr(p);
    expect(p, TokenKind_semicolon);
    struct AstNode* stmt = ast_new(AstNodeKind_expr_stmt);
    stmt->node_expr = e;
    return stmt;
}

struct AstNode* parse_block_stmt(struct Parser* p) {
    struct AstNode* list = ast_new_list(1024);
    expect(p, TokenKind_brace_l);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        struct AstNode* stmt = parse_stmt(p);
        ast_append(list, stmt);
    }
    expect(p, TokenKind_brace_r);
    return list;
}

struct AstNode* parse_stmt(struct Parser* p) {
    struct Token* t = peek_token(p);
    if (t->kind == TokenKind_keyword_return) {
        return parse_return_stmt(p);
    } else if (t->kind == TokenKind_keyword_if) {
        return parse_if_stmt(p);
    } else if (t->kind == TokenKind_keyword_for) {
        return parse_for_stmt(p);
    } else if (t->kind == TokenKind_keyword_while) {
        return parse_while_stmt(p);
    } else if (t->kind == TokenKind_keyword_break) {
        return parse_break_stmt(p);
    } else if (t->kind == TokenKind_keyword_continue) {
        return parse_continue_stmt(p);
    } else if (t->kind == TokenKind_brace_l) {
        return parse_block_stmt(p);
    } else if (is_type_token(t->kind)) {
        return parse_var_decl(p);
    } else {
        return parse_expr_stmt(p);
    }
}

void enter_func(struct Parser* p) {
    p->lvars = calloc(LVAR_MAX, sizeof(struct LVar));
    p->n_lvars = 0;
}

void register_params(struct Parser* p, struct AstNode* params) {
    int i;
    for (i = 0; i < params->node_len; ++i) {
        struct AstNode* param = params->node_items + i;
        p->lvars[p->n_lvars].name = param->name;
        p->lvars[p->n_lvars].ty = param->ty;
        ++p->n_lvars;
    }
}

void register_func(struct Parser* p, char* name, struct Type* ty) {
    p->funcs[p->n_funcs].name = name;
    p->funcs[p->n_funcs].ty = ty;
    ++p->n_funcs;
}

struct AstNode* parse_param(struct Parser* p) {
    struct Type* ty = parse_type(p);
    char* name = NULL;
    enum TokenKind tk = peek_token(p)->kind;
    if (tk != TokenKind_comma && tk != TokenKind_paren_r) {
        name = parse_ident(p);
    }
    struct AstNode* param = ast_new(AstNodeKind_param);
    param->ty = ty;
    param->name = name;
    return param;
}

struct AstNode* parse_param_list(struct Parser* p) {
    int has_void = 0;
    struct AstNode* list = ast_new_list(6);
    while (peek_token(p)->kind != TokenKind_paren_r) {
        struct AstNode* param = parse_param(p);
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

struct AstNode* parse_func_decl_or_def(struct Parser* p) {
    struct Type* ty = parse_type(p);
    char* name = parse_ident(p);
    register_func(p, name, ty);
    expect(p, TokenKind_paren_l);
    struct AstNode* params = parse_param_list(p);
    expect(p, TokenKind_paren_r);
    if (peek_token(p)->kind == TokenKind_semicolon) {
        next_token(p);
        return ast_new(AstNodeKind_func_decl);
    }
    enter_func(p);
    register_params(p, params);
    struct AstNode* body = parse_block_stmt(p);
    struct AstNode* func = ast_new(AstNodeKind_func_def);
    func->ty = ty;
    func->name = name;
    func->node_params = params;
    func->node_body = body;
    return func;
}

struct AstNode* parse_struct_member(struct Parser* p) {
    struct Type* ty = parse_type(p);
    char* name = parse_ident(p);
    expect(p, TokenKind_semicolon);
    struct AstNode* member = ast_new(AstNodeKind_struct_member);
    member->name = name;
    member->ty = ty;
    return member;
}

struct AstNode* parse_struct_members(struct Parser* p) {
    struct AstNode* list = ast_new_list(16);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        struct AstNode* member = parse_struct_member(p);
        ast_append(list, member);
    }
    return list;
}

struct AstNode* parse_struct_decl_or_def(struct Parser* p) {
    expect(p, TokenKind_keyword_struct);
    char* name = parse_ident(p);

    if (peek_token(p)->kind != TokenKind_semicolon && peek_token(p)->kind != TokenKind_brace_l) {
        p->pos = p->pos - 2;
        return parse_func_decl_or_def(p);
    }

    int struct_idx = find_struct(p, name);
    if (struct_idx == -1) {
        struct_idx = p->n_structs;
        p->structs[struct_idx].kind = AstNodeKind_struct_def;
        p->structs[struct_idx].name = name;
        ++p->n_structs;
    }
    if (peek_token(p)->kind == TokenKind_semicolon) {
        next_token(p);
        return ast_new(AstNodeKind_struct_decl);
    }
    if (p->structs[struct_idx].node_members) {
        char* buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_struct_decl_or_def: struct %s redefined", name);
        fatal_error(buf);
    }
    expect(p, TokenKind_brace_l);
    struct AstNode* members = parse_struct_members(p);
    expect(p, TokenKind_brace_r);
    expect(p, TokenKind_semicolon);
    p->structs[struct_idx].node_members = members;
    return p->structs + struct_idx;
}

struct AstNode* parse_enum_member(struct Parser* p) {
    char* name = parse_ident(p);
    struct AstNode* member = ast_new(AstNodeKind_enum_member);
    member->name = name;
    return member;
}

struct AstNode* parse_enum_members(struct Parser* p) {
    int next_value = 0;
    struct AstNode* list = ast_new_list(256);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        struct AstNode* member = parse_enum_member(p);
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

struct AstNode* parse_enum_def(struct Parser* p) {
    expect(p, TokenKind_keyword_enum);
    char* name = parse_ident(p);

    if (peek_token(p)->kind != TokenKind_brace_l) {
        p->pos = p->pos - 2;
        return parse_func_decl_or_def(p);
    }

    int enum_idx = find_enum(p, name);
    if (enum_idx == -1) {
        enum_idx = p->n_enums;
        p->enums[enum_idx].kind = AstNodeKind_enum_def;
        p->enums[enum_idx].name = name;
        ++p->n_enums;
    } else {
        char* buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_enum_def: enum %s redefined", name);
        fatal_error(buf);
    }
    expect(p, TokenKind_brace_l);
    struct AstNode* members = parse_enum_members(p);
    expect(p, TokenKind_brace_r);
    expect(p, TokenKind_semicolon);
    p->enums[enum_idx].node_members = members;
    return p->enums + enum_idx;
}

struct AstNode* parse_toplevel(struct Parser* p) {
    enum TokenKind tk = peek_token(p)->kind;
    if (tk == TokenKind_keyword_struct) {
        return parse_struct_decl_or_def(p);
    } else if (tk == TokenKind_keyword_enum) {
        return parse_enum_def(p);
    } else {
        return parse_func_decl_or_def(p);
    }
}

struct Program* parse(struct Token* tokens) {
    struct Parser* p = parser_new(tokens);
    struct AstNode* list = ast_new_list(1024);
    while (eof(p)) {
        struct AstNode* n = parse_toplevel(p);
        if (n->kind != AstNodeKind_func_def) {
            continue;
        }
        ast_append(list, n);
    }
    struct Program* prog = calloc(1, sizeof(struct Program));
    prog->funcs = list;
    prog->str_literals = p->str_literals;
    return prog;
}

void analyze(struct Program* prog) {
}

enum GenMode {
    GenMode_lval,
    GenMode_rval,
};

struct CodeGen {
    int next_label;
    int* loop_labels;
};

struct CodeGen* codegen_new() {
    struct CodeGen* g = calloc(1, sizeof(struct CodeGen));
    g->next_label = 1;
    g->loop_labels = calloc(1024, sizeof(int));
    return g;
}

int codegen_new_label(struct CodeGen* g) {
    int new_label = g->next_label;
    ++g->next_label;
    return new_label;
}

void codegen_expr(struct CodeGen* g, struct AstNode* ast, enum GenMode gen_mode);
void codegen_stmt(struct CodeGen* g, struct AstNode* ast);

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

void codegen_func_prologue(struct CodeGen* g, struct AstNode* ast) {
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    int i;
    for (i = 0; i < ast->node_params->node_len; ++i) {
        printf("  push %s\n", param_reg(i));
    }
    printf("  sub rsp, %d\n", 8 * LVAR_MAX);
}

void codegen_func_epilogue(struct CodeGen* g, struct AstNode* ast) {
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

void codegen_int_expr(struct CodeGen* g, struct AstNode* ast) {
    printf("  push %d\n", ast->node_int_value);
}

void codegen_str_expr(struct CodeGen* g, struct AstNode* ast) {
    printf("  mov rax, OFFSET FLAG:.Lstr__%d\n", ast->node_idx);
    printf("  push rax\n");
}

void codegen_unary_expr(struct CodeGen* g, struct AstNode* ast) {
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

void codegen_ref_expr(struct CodeGen* g, struct AstNode* ast, enum GenMode gen_mode) {
    codegen_expr(g, ast->node_operand, GenMode_lval);
}

void codegen_lval2rval(struct Type* ty) {
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

void codegen_deref_expr(struct CodeGen* g, struct AstNode* ast, enum GenMode gen_mode) {
    codegen_expr(g, ast->node_operand, GenMode_rval);
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(ast->node_operand->ty->to);
    }
}

void codegen_logical_expr(struct CodeGen* g, struct AstNode* ast) {
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

void codegen_binary_expr(struct CodeGen* g, struct AstNode* ast, enum GenMode gen_mode) {
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

void codegen_assign_expr(struct CodeGen* g, struct AstNode* ast) {
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

void codegen_func_call(struct CodeGen* g, struct AstNode* ast) {
    char* func_name = ast->name;
    struct AstNode* args = ast->node_args;
    int i;
    for (i = 0; i < args->node_len; ++i) {
        struct AstNode* arg = args->node_items + i;
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
    printf("  call %s\n", func_name);
    printf("  add rsp, 8\n");
    printf("  push rax\n");

    printf("  jmp .Lend%d\n", label);
    printf(".Laligned%d:\n", label);

    printf("  mov rax, 0\n");
    printf("  call %s\n", func_name);
    printf("  push rax\n");

    printf(".Lend%d:\n", label);
}

void codegen_lvar(struct CodeGen* g, struct AstNode* ast, enum GenMode gen_mode) {
    int offset = 8 + ast->node_idx * 8;
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", offset);
    printf("  push rax\n");
    if (gen_mode == GenMode_rval) {
        codegen_lval2rval(ast->ty);
    }
}

void codegen_expr(struct CodeGen* g, struct AstNode* ast, enum GenMode gen_mode) {
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
    } else {
        unreachable();
    }
}

void codegen_return_stmt(struct CodeGen* g, struct AstNode* ast) {
    if (ast->node_expr) {
        codegen_expr(g, ast->node_expr, GenMode_rval);
        printf("  pop rax\n");
    }
    codegen_func_epilogue(g, ast);
}

void codegen_if_stmt(struct CodeGen* g, struct AstNode* ast) {
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

void codegen_for_stmt(struct CodeGen* g, struct AstNode* ast) {
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

void codegen_break_stmt(struct CodeGen* g, struct AstNode* ast) {
    int label = *g->loop_labels;
    printf("  jmp .Lend%d\n", label);
}

void codegen_continue_stmt(struct CodeGen* g, struct AstNode* ast) {
    int label = *g->loop_labels;
    printf("  jmp .Lcontinue%d\n", label);
}

void codegen_expr_stmt(struct CodeGen* g, struct AstNode* ast) {
    codegen_expr(g, ast->node_expr, GenMode_rval);
    printf("  pop rax\n");
}

void codegen_var_decl(struct CodeGen* g, struct AstNode* ast) {
}

void codegen_block_stmt(struct CodeGen* g, struct AstNode* ast) {
    int i;
    for (i = 0; i < ast->node_len; ++i) {
        struct AstNode* stmt = ast->node_items + i;
        codegen_stmt(g, stmt);
    }
}

void codegen_stmt(struct CodeGen* g, struct AstNode* ast) {
    if (ast->kind == AstNodeKind_list) {
        codegen_block_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_return_stmt) {
        codegen_return_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_if_stmt) {
        codegen_if_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_for_stmt) {
        codegen_for_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_break_stmt) {
        codegen_break_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_continue_stmt) {
        codegen_continue_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_expr_stmt) {
        codegen_expr_stmt(g, ast);
    } else if (ast->kind == AstNodeKind_var_decl) {
        codegen_var_decl(g, ast);
    } else {
        unreachable();
    }
}

void codegen_func(struct CodeGen* g, struct AstNode* ast) {
    printf("%s:\n", ast->name);

    codegen_func_prologue(g, ast);
    codegen_stmt(g, ast->node_body);
    codegen_func_epilogue(g, ast);

    printf("\n");
}

void codegen(struct Program* prog) {
    struct CodeGen* g = codegen_new();

    printf(".intel_syntax noprefix\n\n");

    int i;
    for (i = 0; prog->str_literals[i]; ++i) {
        printf(".Lstr__%d:\n", i + 1);
        printf("  .string \"%s\"\n\n", prog->str_literals[i]);
    }

    printf(".globl main\n\n");

    for (i = 0; i < prog->funcs->node_len; ++i) {
        struct AstNode* func = prog->funcs->node_items + i;
        codegen_func(g, func);
    }
}

int main() {
    char* source = read_all();
    struct Token* tokens = tokenize(source);
    struct Program* prog = parse(tokens);
    analyze(prog);
    codegen(prog);
    return 0;
}
