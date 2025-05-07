int atoi();
void* calloc();
void exit();
int getchar();
int isalnum();
int isalpha();
int isdigit();
int isspace();
void* memcpy();
int printf();
int sprintf();
int strcmp();
char* strstr();

#define NULL 0

void fatal_error(char* msg) {
    printf("%s\n", msg);
    exit(1);
}

void unreachable() {
    fatal_error("unreachable");
}

void read_all(char* buf) {
    while (1) {
        int c = getchar();
        if (c == -1) {
            break;
        }
        *buf = c;
        ++buf;
    }
}

#define TK_EOF        0

#define TK_AND        1
#define TK_ANDAND     2
#define TK_ARROW      3
#define TK_ASSIGN     4
#define TK_ASSIGN_ADD 5
#define TK_ASSIGN_SUB 6
#define TK_BRACE_L    7
#define TK_BRACE_R    8
#define TK_BRACKET_L  9
#define TK_BRACKET_R  10
#define TK_COMMA      11
#define TK_DOT        12
#define TK_EQ         13
#define TK_GE         14
#define TK_GT         15
#define TK_IDENT      16
#define TK_K_BREAK    17
#define TK_K_CHAR     18
#define TK_K_CONTINUE 19
#define TK_K_ELSE     20
#define TK_K_FOR      21
#define TK_K_IF       22
#define TK_K_INT      23
#define TK_K_LONG     24
#define TK_K_RETURN   25
#define TK_K_SIZEOF   26
#define TK_K_STRUCT   27
#define TK_K_VOID     28
#define TK_K_WHILE    29
#define TK_LE         30
#define TK_LT         31
#define TK_L_INT      32
#define TK_L_STR      33
#define TK_MINUS      34
#define TK_MINUSMINUS 35
#define TK_NE         36
#define TK_NOT        37
#define TK_OROR       38
#define TK_PAREN_L    39
#define TK_PAREN_R    40
#define TK_PERCENT    41
#define TK_PLUS       42
#define TK_PLUSPLUS   43
#define TK_SEMICOLON  44
#define TK_SLASH      45
#define TK_STAR       46

struct Token {
    int kind;
    char* value;
};

struct Define {
    char* from;
    struct Token* to;
};

struct Token* tokenize(char* src) {
    struct Token* tokens = calloc(1024*1024, sizeof(struct Token));
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
            tok->kind = TK_PAREN_L;
        } else if (c == ')') {
            tok->kind = TK_PAREN_R;
        } else if (c == '{') {
            tok->kind = TK_BRACE_L;
        } else if (c == '}') {
            tok->kind = TK_BRACE_R;
        } else if (c == '[') {
            tok->kind = TK_BRACKET_L;
        } else if (c == ']') {
            tok->kind = TK_BRACKET_R;
        } else if (c == ',') {
            tok->kind = TK_COMMA;
        } else if (c == ';') {
            tok->kind = TK_SEMICOLON;
        } else if (c == '+') {
            if (src[pos] == '=') {
                ++pos;
                tok->kind = TK_ASSIGN_ADD;
            } else if (src[pos] == '+') {
                ++pos;
                tok->kind = TK_PLUSPLUS;
            } else {
                tok->kind = TK_PLUS;
            }
        } else if (c == '|') {
            ++pos;
            tok->kind = TK_OROR;
        } else if (c == '&') {
            if (src[pos] == '&') {
                ++pos;
                tok->kind = TK_ANDAND;
            } else {
                tok->kind = TK_AND;
            }
        } else if (c == '-') {
            if (src[pos] == '>') {
                ++pos;
                tok->kind = TK_ARROW;
            } else if (src[pos] == '=') {
                ++pos;
                tok->kind = TK_ASSIGN_SUB;
            } else if (src[pos] == '-') {
                ++pos;
                tok->kind = TK_MINUSMINUS;
            } else {
                tok->kind = TK_MINUS;
            }
        } else if (c == '*') {
            tok->kind = TK_STAR;
        } else if (c == '/') {
            tok->kind = TK_SLASH;
        } else if (c == '%') {
            tok->kind = TK_PERCENT;
        } else if (c == '.') {
            tok->kind = TK_DOT;
        } else if (c == '!') {
            if (src[pos] == '=') {
                ++pos;
                tok->kind = TK_NE;
            } else {
                tok->kind = TK_NOT;
            }
        } else if (c == '=') {
            if (src[pos] == '=') {
                ++pos;
                tok->kind = TK_EQ;
            } else {
                tok->kind = TK_ASSIGN;
            }
        } else if (c == '<') {
            if (src[pos] == '=') {
                ++pos;
                tok->kind = TK_LE;
            } else {
                tok->kind = TK_LT;
            }
        } else if (c == '>') {
            if (src[pos] == '=') {
                ++pos;
                tok->kind = TK_GE;
            } else {
                tok->kind = TK_GT;
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
            tok->kind = TK_L_INT;
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
            tok->kind = TK_L_STR;
            tok->value = calloc(pos - start + 1, sizeof(char));
            memcpy(tok->value, src + start, pos - start);
            ++pos;
        } else if (isdigit(c)) {
            --pos;
            start = pos;
            while (isdigit(src[pos])) {
                ++pos;
            }
            tok->kind = TK_L_INT;
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
                tok->kind = TK_K_BREAK;
            } else if (ident_len == 4 && strstr(src + start, "char") == src + start) {
                tok->kind = TK_K_CHAR;
            } else if (ident_len == 8 && strstr(src + start, "continue") == src + start) {
                tok->kind = TK_K_CONTINUE;
            } else if (ident_len == 4 && strstr(src + start, "else") == src + start) {
                tok->kind = TK_K_ELSE;
            } else if (ident_len == 3 && strstr(src + start, "for") == src + start) {
                tok->kind = TK_K_FOR;
            } else if (ident_len == 2 && strstr(src + start, "if") == src + start) {
                tok->kind = TK_K_IF;
            } else if (ident_len == 3 && strstr(src + start, "int") == src + start) {
                tok->kind = TK_K_INT;
            } else if (ident_len == 4 && strstr(src + start, "long") == src + start) {
                tok->kind = TK_K_LONG;
            } else if (ident_len == 6 && strstr(src + start, "return") == src + start) {
                tok->kind = TK_K_RETURN;
            } else if (ident_len == 6 && strstr(src + start, "sizeof") == src + start) {
                tok->kind = TK_K_SIZEOF;
            } else if (ident_len == 6 && strstr(src + start, "struct") == src + start) {
                tok->kind = TK_K_STRUCT;
            } else if (ident_len == 4 && strstr(src + start, "void") == src + start) {
                tok->kind = TK_K_VOID;
            } else if (ident_len == 5 && strstr(src + start, "while") == src + start) {
                tok->kind = TK_K_WHILE;
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
                    tok->kind = TK_IDENT;
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
                def->to->kind = TK_L_INT;
            } else {
                def->to->kind = TK_IDENT;
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

#define TY_UNKNOWN 0

#define TY_CHAR    1
#define TY_INT     2
#define TY_LONG    3
#define TY_VOID    4
#define TY_PTR     5
#define TY_STRUCT  6

struct AstNode;

struct Type {
    int kind;
    struct Type* to;
    struct AstNode* struct_def;
};

struct Type* type_new(int kind) {
    struct Type* ty = calloc(1, sizeof(struct Type));
    ty->kind = kind;
    return ty;
}

struct Type* type_new_ptr(struct Type* to) {
    struct Type* ty = calloc(1, sizeof(struct Type));
    ty->kind = TY_PTR;
    ty->to = to;
    return ty;
}

int type_is_unsized(struct Type* ty) {
    return ty->kind != TY_VOID;
}

int type_sizeof_struct(struct Type* ty);
int type_alignof_struct(struct Type* ty);
int type_offsetof(struct Type* ty, char* name);
struct Type* type_member_typeof(struct Type* ty, char* name);

int type_sizeof(struct Type* ty) {
    if (!type_is_unsized(ty)) {
        fatal_error("type_sizeof: type size cannot be determined");
    }

    if (ty->kind == TY_PTR) {
        return 8;
    } else if (ty->kind == TY_CHAR) {
        return 1;
    } else if (ty->kind == TY_INT) {
        return 4;
    } else if (ty->kind == TY_LONG) {
        return 8;
    } else {
        return type_sizeof_struct(ty);
    }
}

int type_alignof(struct Type* ty) {
    if (!type_is_unsized(ty)) {
        fatal_error("type_alignof: type size cannot be determined");
    }

    if (ty->kind == TY_PTR) {
        return 8;
    } else if (ty->kind == TY_CHAR) {
        return 1;
    } else if (ty->kind == TY_INT) {
        return 4;
    } else if (ty->kind == TY_LONG) {
        return 8;
    } else {
        return type_alignof_struct(ty);
    }
}

#define AST_UNKNOWN            0

#define AST_ASSIGN_EXPR   1
#define AST_BINARY_EXPR   2
#define AST_BREAK_STMT    3
#define AST_CONTINUE_STMT 4
#define AST_DEREF_EXPR    5
#define AST_EXPR_STMT     6
#define AST_FOR_STMT      7
#define AST_FUNC_CALL     8
#define AST_FUNC_DECL     9
#define AST_FUNC_DEF      10
#define AST_IF_STMT       11
#define AST_INT_EXPR      12
#define AST_LIST          13
#define AST_LOGICAL_EXPR  14
#define AST_LVAR          15
#define AST_PARAM         16
#define AST_REF_EXPR      17
#define AST_RETURN_STMT   18
#define AST_STRUCT_DECL   19
#define AST_STRUCT_DEF    20
#define AST_STRUCT_MEMBER 21
#define AST_STR_EXPR      22
#define AST_TYPE          23
#define AST_UNARY_EXPR    24
#define AST_VAR_DECL      25

#define node_items     __n1
#define node_len       __i
#define node_expr      __n1
#define node_lhs       __n1
#define node_rhs       __n2
#define node_operand   __n1
#define node_cond      __n1
#define node_init      __n2
#define node_update    __n3
#define node_then      __n2
#define node_else      __n3
#define node_body      __n4
#define node_members   __n1
#define node_params    __n1
#define node_args      __n1
#define node_int_value __i
#define node_idx       __i
#define node_op        __i

struct AstNode {
    int kind;
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

struct AstNode* ast_new(int kind) {
    struct AstNode* ast = calloc(1, sizeof(struct AstNode));
    ast->kind = kind;
    return ast;
}

struct AstNode* ast_new_list(int capacity) {
    struct AstNode* list = ast_new(AST_LIST);
    list->node_items = calloc(capacity, sizeof(struct AstNode));
    list->node_len = 0;
    return list;
}

void ast_append(struct AstNode* list, struct AstNode* item) {
    if (list->kind != AST_LIST) {
        fatal_error("ast_append: ast is not a list");
    }
    if (!item) {
        return;
    }
    memcpy(list->node_items + list->node_len, item, sizeof(struct AstNode));
    ++list->node_len;
}

struct AstNode* ast_new_int(int v) {
    struct AstNode* e = ast_new(AST_INT_EXPR);
    e->node_int_value = v;
    e->ty = type_new(TY_INT);
    return e;
}

struct AstNode* ast_new_unary_expr(int op, struct AstNode* operand) {
    struct AstNode* e = ast_new(AST_UNARY_EXPR);
    e->node_op = op;
    e->node_operand = operand;
    e->ty = type_new(TY_INT);
    return e;
}

struct AstNode* ast_new_binary_expr(int op, struct AstNode* lhs, struct AstNode* rhs) {
    struct AstNode* e = ast_new(AST_BINARY_EXPR);
    e->node_op = op;
    e->node_lhs = lhs;
    e->node_rhs = rhs;
    if (op == TK_PLUS) {
        if (lhs->ty->kind == TY_PTR) {
            e->ty = lhs->ty;
        } else if (rhs->ty->kind == TY_PTR) {
            e->ty = rhs->ty;
        } else {
            e->ty = type_new(TY_INT);
        }
    } else if (op == TK_MINUS) {
        if (lhs->ty->kind == TY_PTR) {
            e->ty = lhs->ty;
        } else {
            e->ty = type_new(TY_INT);
        }
    } else {
        e->ty = type_new(TY_INT);
    }
    return e;
}

struct AstNode* ast_new_assign_expr(int op, struct AstNode* lhs, struct AstNode* rhs) {
    struct AstNode* e = ast_new(AST_ASSIGN_EXPR);
    e->node_op = op;
    e->node_lhs = lhs;
    e->node_rhs = rhs;
    e->ty = lhs->ty;
    return e;
}

struct AstNode* ast_new_assign_add_expr(struct AstNode* lhs, struct AstNode* rhs) {
    if (lhs->ty->kind == TY_PTR) {
        rhs = ast_new_binary_expr(TK_STAR, rhs, ast_new_int(type_sizeof(lhs->ty->to)));
    } else if (rhs->ty->kind == TY_PTR) {
        lhs = ast_new_binary_expr(TK_STAR, lhs, ast_new_int(type_sizeof(rhs->ty->to)));
    }
    return ast_new_assign_expr(TK_ASSIGN_ADD, lhs, rhs);
}

struct AstNode* ast_new_assign_sub_expr(struct AstNode* lhs, struct AstNode* rhs) {
    if (lhs->ty->kind == TY_PTR) {
        rhs = ast_new_binary_expr(TK_STAR, rhs, ast_new_int(type_sizeof(lhs->ty->to)));
    }
    return ast_new_assign_expr(TK_ASSIGN_SUB, lhs, rhs);
}

struct AstNode* ast_new_ref_expr(struct AstNode* operand) {
    struct AstNode* e = ast_new(AST_REF_EXPR);
    e->node_operand = operand;
    e->ty = type_new_ptr(operand->ty);
    return e;
}

struct AstNode* ast_new_deref_expr(struct AstNode* operand) {
    struct AstNode* e = ast_new(AST_DEREF_EXPR);
    e->node_operand = operand;
    e->ty = operand->ty->to;
    return e;
}

struct AstNode* ast_new_member_access_expr(struct AstNode* obj, char* name) {
    struct AstNode* e = ast_new(AST_DEREF_EXPR);
    e->node_operand = ast_new_binary_expr(TK_PLUS, obj, ast_new_int(type_offsetof(obj->ty->to, name)));
    e->ty = type_member_typeof(obj->ty->to, name);
    return e;
}

int type_sizeof_struct(struct Type* ty) {
    int next_offset = 0;
    int struct_align = 0;
    int padding;

    int i;
    for (i = 0; i < ty->struct_def->node_members->node_len; ++i) {
        struct AstNode* member = ty->struct_def->node_members->node_items + i;
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
    for (i = 0; i < ty->struct_def->node_members->node_len; ++i) {
        struct AstNode* member = ty->struct_def->node_members->node_items + i;
        int align = type_alignof(member->ty);

        if (struct_align < align) {
            struct_align = align;
        }
    }
    return struct_align;
}

int type_offsetof(struct Type* ty, char* name) {
    if (ty->kind != TY_STRUCT) {
        fatal_error("type_offsetof: type is not a struct");
    }

    int next_offset = 0;

    int i;
    for (i = 0; i < ty->struct_def->node_members->node_len; ++i) {
        struct AstNode* member = ty->struct_def->node_members->node_items + i;
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

struct Type* type_member_typeof(struct Type* ty, char* name) {
    if (ty->kind != TY_STRUCT) {
        fatal_error("type_offsetof: type is not a struct");
    }

    int i;
    for (i = 0; i < ty->struct_def->node_members->node_len; ++i) {
        struct AstNode* member = ty->struct_def->node_members->node_items + i;
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
    char** str_literals;
    int n_str_literals;
};

struct Parser* parser_new(struct Token* tokens) {
    struct Parser* p = calloc(1, sizeof(struct Parser));
    p->tokens = tokens;
    p->funcs = calloc(128, sizeof(struct Func));
    p->structs = calloc(64, sizeof(struct AstNode));
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
    return peek_token(p)->kind != TK_EOF;
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

int find_lvar(struct Parser* p, char* name) {
    int i;
    for (i = 0; i < p->n_lvars; ++i) {
        if (strcmp(p->lvars[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_func(struct Parser* p, char* name) {
    int i;
    for (i = 0; i < p->n_funcs; ++i) {
        if (strcmp(p->funcs[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_struct(struct Parser* p, char* name) {
    int i;
    for (i = 0; i < p->n_structs; ++i) {
        if (strcmp(p->structs[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

struct AstNode* parse_expr(struct Parser* p);
struct AstNode* parse_stmt(struct Parser* p);

char* parse_ident(struct Parser* p) {
    return expect(p, TK_IDENT)->value;
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
    if (t->kind == TK_L_INT) {
        return ast_new_int(atoi(t->value));
    } else if (t->kind == TK_L_STR) {
        e = ast_new(AST_STR_EXPR);
        e->node_idx = register_str_literal(p, t->value);
        return e;
    } else if (t->kind == TK_PAREN_L) {
        e = parse_expr(p);
        expect(p, TK_PAREN_R);
        return e;
    } else if (t->kind == TK_IDENT) {
        char* name = t->value;

        if (peek_token(p)->kind == TK_PAREN_L) {
            e = ast_new(AST_FUNC_CALL);
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
            buf = calloc(1024, sizeof(char));
            sprintf(buf, "undefined variable: %s", name);
            fatal_error(buf);
        }

        e = ast_new(AST_LVAR);
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
    while (peek_token(p)->kind != TK_PAREN_R) {
        struct AstNode* arg = parse_expr(p);
        ast_append(list, arg);
        if (peek_token(p)->kind == TK_COMMA) {
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
        int tk = peek_token(p)->kind;
        if (tk == TK_PAREN_L) {
            next_token(p);
            struct AstNode* args = parse_arg_list(p);
            expect(p, TK_PAREN_R);
            ret->node_args = args;
        } else if (tk == TK_BRACKET_L) {
            next_token(p);
            struct AstNode* idx = parse_expr(p);
            expect(p, TK_BRACKET_R);
            idx = ast_new_binary_expr(TK_STAR, idx, ast_new_int(type_sizeof(ret->ty->to)));
            ret = ast_new_deref_expr(ast_new_binary_expr(TK_PLUS, ret, idx));
        } else if (tk == TK_DOT) {
            next_token(p);
            name = parse_ident(p);
            ret = ast_new_member_access_expr(ast_new_ref_expr(ret), name);
        } else if (tk == TK_ARROW) {
            next_token(p);
            name = parse_ident(p);
            ret = ast_new_member_access_expr(ret, name);
        } else {
            break;
        }
    }
    return ret;
}

int is_type_token(int token_kind) {
    return token_kind == TK_K_INT || token_kind == TK_K_LONG || token_kind == TK_K_CHAR || token_kind == TK_K_VOID || token_kind == TK_K_STRUCT;
}

struct Type* parse_type(struct Parser* p) {
    struct Token* t = next_token(p);
    char* buf;
    if (!is_type_token(t->kind)) {
        buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_type: unknown type, %d", t->kind);
        fatal_error(buf);
    }
    struct Type* ty = type_new(TY_UNKNOWN);
    if (t->kind == TK_K_INT) {
        ty->kind = TY_INT;
    } else if (t->kind == TK_K_LONG) {
        ty->kind = TY_LONG;
    } else if (t->kind == TK_K_CHAR) {
        ty->kind = TY_CHAR;
    } else if (t->kind == TK_K_VOID) {
        ty->kind = TY_VOID;
    } else if (t->kind == TK_K_STRUCT) {
        ty->kind = TY_STRUCT;
        char* name = parse_ident(p);
        int struct_idx = find_struct(p, name);
        if (struct_idx == -1) {
            buf = calloc(1024, sizeof(char));
            sprintf(buf, "parse_type: unknown struct, %s", name);
            fatal_error(buf);
        }
        ty->struct_def = p->structs + struct_idx;
    } else {
        unreachable();
    }
    while (1) {
        if (peek_token(p)->kind == TK_STAR) {
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
    int op = peek_token(p)->kind;
    if (op == TK_MINUS) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_binary_expr(op, ast_new_int(0), operand);
    } else if (op == TK_NOT) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_unary_expr(op, operand);
    } else if (op == TK_AND) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_ref_expr(operand);
    } else if (op == TK_STAR) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_deref_expr(operand);
    } else if (op == TK_PLUSPLUS) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_assign_add_expr(operand, ast_new_int(1));
    } else if (op == TK_MINUSMINUS) {
        next_token(p);
        operand = parse_prefix_expr(p);
        return ast_new_assign_sub_expr(operand, ast_new_int(1));
    } else if (op == TK_K_SIZEOF) {
        next_token(p);
        expect(p, TK_PAREN_L);
        struct Type* ty = parse_type(p);
        expect(p, TK_PAREN_R);
        return ast_new_int(type_sizeof(ty));
    }
    return parse_postfix_expr(p);
}

struct AstNode* parse_multiplicative_expr(struct Parser* p) {
    struct AstNode* lhs = parse_prefix_expr(p);
    while (1) {
        int op = peek_token(p)->kind;
        if (op == TK_STAR || op == TK_SLASH || op == TK_PERCENT) {
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
        int op = peek_token(p)->kind;
        if (op == TK_PLUS) {
            next_token(p);
            rhs = parse_multiplicative_expr(p);
            if (lhs->ty->kind == TY_PTR) {
                lhs = ast_new_binary_expr(op, lhs, ast_new_binary_expr(TK_STAR, rhs, ast_new_int(type_sizeof(lhs->ty->to))));
            } else if (rhs->ty->kind == TY_PTR) {
                lhs = ast_new_binary_expr(op, ast_new_binary_expr(TK_STAR, lhs, ast_new_int(type_sizeof(rhs->ty->to))), rhs);
            } else {
                lhs = ast_new_binary_expr(op, lhs, rhs);
            }
        } else if (op == TK_MINUS) {
            next_token(p);
            rhs = parse_multiplicative_expr(p);
            if (lhs->ty->kind == TY_PTR) {
                lhs = ast_new_binary_expr(op, lhs, ast_new_binary_expr(TK_STAR, rhs, ast_new_int(type_sizeof(lhs->ty->to))));
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
        int op = peek_token(p)->kind;
        if (op == TK_LT || op == TK_LE) {
            next_token(p);
            rhs = parse_additive_expr(p);
            lhs = ast_new_binary_expr(op, lhs, rhs);
        } else if (op == TK_GT) {
            next_token(p);
            rhs = parse_additive_expr(p);
            lhs = ast_new_binary_expr(TK_LT, rhs, lhs);
        } else if (op == TK_GE) {
            next_token(p);
            rhs = parse_additive_expr(p);
            lhs = ast_new_binary_expr(TK_LE, rhs, lhs);
        } else {
            break;
        }
    }
    return lhs;
}

struct AstNode* parse_equality_expr(struct Parser* p) {
    struct AstNode* lhs = parse_relational_expr(p);
    while (1) {
        int op = peek_token(p)->kind;
        if (op == TK_EQ || op == TK_NE) {
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
        int op = peek_token(p)->kind;
        if (op == TK_ANDAND) {
            next_token(p);
            struct AstNode* rhs = parse_equality_expr(p);
            struct AstNode* e = ast_new(AST_LOGICAL_EXPR);
            e->node_op = op;
            e->node_lhs = lhs;
            e->node_rhs = rhs;
            e->ty = type_new(TY_INT);
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
        int op = peek_token(p)->kind;
        if (op == TK_OROR) {
            next_token(p);
            struct AstNode* rhs = parse_logical_and_expr(p);
            struct AstNode* e = ast_new(AST_LOGICAL_EXPR);
            e->node_op = op;
            e->node_lhs = lhs;
            e->node_rhs = rhs;
            e->ty = type_new(TY_INT);
            lhs = e;
        } else {
            break;
        }
    }
    return lhs;
}

struct AstNode* parse_assignment_expr(struct Parser *p) {
    struct AstNode* lhs = parse_logical_or_expr(p);
    struct AstNode* rhs;
    while (1) {
        int op = peek_token(p)->kind;
        if (op == TK_ASSIGN) {
            next_token(p);
            rhs = parse_logical_or_expr(p);
            lhs = ast_new_assign_expr(op, lhs, rhs);
        } else if (op == TK_ASSIGN_ADD) {
            next_token(p);
            rhs = parse_logical_or_expr(p);
            lhs = ast_new_assign_add_expr(lhs, rhs);
        } else if (op == TK_ASSIGN_SUB) {
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
    expect(p, TK_K_RETURN);
    if (peek_token(p)->kind == TK_SEMICOLON) {
        next_token(p);
        return ast_new(AST_RETURN_STMT);
    }

    struct AstNode* expr = parse_expr(p);
    expect(p, TK_SEMICOLON);

    struct AstNode* ret = ast_new(AST_RETURN_STMT);
    ret->node_expr = expr;
    return ret;
}

struct AstNode* parse_if_stmt(struct Parser* p) {
    expect(p, TK_K_IF);
    expect(p, TK_PAREN_L);
    struct AstNode* cond = parse_expr(p);
    expect(p, TK_PAREN_R);
    struct AstNode* then_body = parse_stmt(p);
    struct AstNode* else_body = NULL;
    if (peek_token(p)->kind == TK_K_ELSE) {
        next_token(p);
        else_body = parse_stmt(p);
    }

    struct AstNode* stmt = ast_new(AST_IF_STMT);
    stmt->node_cond = cond;
    stmt->node_then = then_body;
    stmt->node_else = else_body;
    return stmt;
}

struct AstNode* parse_for_stmt(struct Parser* p) {
    expect(p, TK_K_FOR);
    expect(p, TK_PAREN_L);
    struct AstNode* init = NULL;
    struct AstNode* cond = NULL;
    struct AstNode* update = NULL;
    if (peek_token(p)->kind != TK_SEMICOLON) {
        init = parse_expr(p);
    }
    expect(p, TK_SEMICOLON);
    if (peek_token(p)->kind != TK_SEMICOLON) {
        cond = parse_expr(p);
    } else {
        cond = ast_new_int(1);
    }
    expect(p, TK_SEMICOLON);
    if (peek_token(p)->kind != TK_PAREN_R) {
        update = parse_expr(p);
    }
    expect(p, TK_PAREN_R);
    struct AstNode* body = parse_stmt(p);

    struct AstNode* stmt = ast_new(AST_FOR_STMT);
    stmt->node_cond = cond;
    stmt->node_init = init;
    stmt->node_update = update;
    stmt->node_body = body;
    return stmt;
}

struct AstNode* parse_while_stmt(struct Parser* p) {
    expect(p, TK_K_WHILE);
    expect(p, TK_PAREN_L);
    struct AstNode* cond = parse_expr(p);
    expect(p, TK_PAREN_R);
    struct AstNode* body = parse_stmt(p);

    struct AstNode* stmt = ast_new(AST_FOR_STMT);
    stmt->node_cond = cond;
    stmt->node_body = body;
    return stmt;
}

struct AstNode* parse_break_stmt(struct Parser* p) {
    expect(p, TK_K_BREAK);
    expect(p, TK_SEMICOLON);
    return ast_new(AST_BREAK_STMT);
}

struct AstNode* parse_continue_stmt(struct Parser* p) {
    expect(p, TK_K_CONTINUE);
    expect(p, TK_SEMICOLON);
    return ast_new(AST_CONTINUE_STMT);
}

struct AstNode* parse_var_decl(struct Parser* p) {
    struct Type* ty = parse_type(p);
    if (!type_is_unsized(ty)) {
        fatal_error("parse_var_decl: invalid type for variable");
    }
    char* name = parse_ident(p);

    struct AstNode* init = NULL;
    if (peek_token(p)->kind == TK_ASSIGN) {
        next_token(p);
        init = parse_expr(p);
    }
    expect(p, TK_SEMICOLON);

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
        struct AstNode* lhs = ast_new(AST_LVAR);
        lhs->name = name;
        lhs->node_idx = p->n_lvars - 1;
        lhs->ty = ty;
        struct AstNode* assign = ast_new_assign_expr(TK_ASSIGN, lhs, init);
        ret = ast_new(AST_EXPR_STMT);
        ret->node_expr = assign;
    } else {
        ret = ast_new(AST_VAR_DECL);
    }
    return ret;
}

struct AstNode* parse_expr_stmt(struct Parser* p) {
    struct AstNode* e = parse_expr(p);
    expect(p, TK_SEMICOLON);
    struct AstNode* stmt = ast_new(AST_EXPR_STMT);
    stmt->node_expr = e;
    return stmt;
}

struct AstNode* parse_block_stmt(struct Parser* p) {
    struct AstNode* list = ast_new_list(1024);
    expect(p, TK_BRACE_L);
    while (peek_token(p)->kind != TK_BRACE_R) {
        struct AstNode* stmt = parse_stmt(p);
        ast_append(list, stmt);
    }
    expect(p, TK_BRACE_R);
    return list;
}

struct AstNode* parse_stmt(struct Parser* p) {
    struct Token* t = peek_token(p);
    if (t->kind == TK_K_RETURN) {
        return parse_return_stmt(p);
    } else if (t->kind == TK_K_IF) {
        return parse_if_stmt(p);
    } else if (t->kind == TK_K_FOR) {
        return parse_for_stmt(p);
    } else if (t->kind == TK_K_WHILE) {
        return parse_while_stmt(p);
    } else if (t->kind == TK_K_BREAK) {
        return parse_break_stmt(p);
    } else if (t->kind == TK_K_CONTINUE) {
        return parse_continue_stmt(p);
    } else if (t->kind == TK_BRACE_L) {
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
    if (!type_is_unsized(ty)) {
        fatal_error("parse_param: invalid type for variable");
    }
    char* name = parse_ident(p);
    struct AstNode* param = ast_new(AST_PARAM);
    param->ty = ty;
    param->name = name;
    return param;
}

struct AstNode* parse_param_list(struct Parser* p) {
    struct AstNode* list = ast_new_list(6);
    while (peek_token(p)->kind != TK_PAREN_R) {
        struct AstNode* param = parse_param(p);
        ast_append(list, param);
        if (peek_token(p)->kind == TK_COMMA) {
            next_token(p);
        } else {
            break;
        }
    }
    if (list->node_len > 6) {
        fatal_error("too many parameters");
    }
    return list;
}

struct AstNode* parse_func_decl_or_def(struct Parser* p) {
    struct Type* ty = parse_type(p);
    char* name = parse_ident(p);
    register_func(p, name, ty);
    expect(p, TK_PAREN_L);
    struct AstNode* params = parse_param_list(p);
    expect(p, TK_PAREN_R);
    if (peek_token(p)->kind == TK_SEMICOLON) {
        next_token(p);
        return ast_new(AST_FUNC_DECL);
    }
    enter_func(p);
    register_params(p, params);
    struct AstNode* body = parse_block_stmt(p);
    struct AstNode* func = ast_new(AST_FUNC_DEF);
    func->ty = ty;
    func->name = name;
    func->node_params = params;
    func->node_body = body;
    return func;
}

struct AstNode* parse_struct_member(struct Parser* p) {
    struct Type* ty = parse_type(p);
    char* name = parse_ident(p);
    expect(p, TK_SEMICOLON);
    struct AstNode* member = ast_new(AST_STRUCT_MEMBER);
    member->name = name;
    member->ty = ty;
    return member;
}

struct AstNode* parse_struct_members(struct Parser* p) {
    struct AstNode* list = ast_new_list(16);
    while (peek_token(p)->kind != TK_BRACE_R) {
        struct AstNode* member = parse_struct_member(p);
        ast_append(list, member);
    }
    return list;
}

struct AstNode* parse_struct_decl_or_def(struct Parser* p) {
    expect(p, TK_K_STRUCT);
    char* name = parse_ident(p);

    if (peek_token(p)->kind != TK_SEMICOLON && peek_token(p)->kind != TK_BRACE_L) {
        p->pos = p->pos - 2;
        return parse_func_decl_or_def(p);
    }

    int struct_idx = find_struct(p, name);
    if (struct_idx == -1) {
        struct_idx = p->n_structs;
        p->structs[struct_idx].kind = AST_STRUCT_DEF;
        p->structs[struct_idx].name = name;
        ++p->n_structs;
    }
    if (peek_token(p)->kind == TK_SEMICOLON) {
        next_token(p);
        return ast_new(AST_STRUCT_DECL);
    }
    if (p->structs[struct_idx].node_members) {
        char* buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_struct_decl_or_def: struct %s redefined", name);
        fatal_error(buf);
    }
    expect(p, TK_BRACE_L);
    struct AstNode* members = parse_struct_members(p);
    expect(p, TK_BRACE_R);
    expect(p, TK_SEMICOLON);
    p->structs[struct_idx].node_members = members;
    return p->structs + struct_idx;
}

struct AstNode* parse_toplevel(struct Parser* p) {
    if (peek_token(p)->kind == TK_K_STRUCT) {
        return parse_struct_decl_or_def(p);
    } else {
        return parse_func_decl_or_def(p);
    }
}

struct Program* parse(struct Parser* p) {
    struct AstNode* list = ast_new_list(1024);
    while (eof(p)) {
        struct AstNode* n = parse_toplevel(p);
        if (n->kind != AST_FUNC_DEF) {
            continue;
        }
        ast_append(list, n);
    }
    struct Program* prog = calloc(1, sizeof(struct Program));
    prog->funcs = list;
    prog->str_literals = p->str_literals;
    return prog;
}

#define GEN_LVAL 0
#define GEN_RVAL 1

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

int gen_new_label(struct CodeGen* g) {
    int new_label = g->next_label;
    ++g->next_label;
    return new_label;
}

void gen_expr(struct CodeGen* g, struct AstNode* ast, int gen_mode);
void gen_stmt(struct CodeGen* g, struct AstNode* ast);

char* param_reg(int n) {
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

void gen_func_prologue(struct CodeGen* g, struct AstNode* ast) {
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    int i;
    for (i = 0; i < ast->node_params->node_len; ++i) {
        printf("  push %s\n", param_reg(i));
    }
    printf("  sub rsp, %d\n", 8 * LVAR_MAX);
}

void gen_func_epilogue(struct CodeGen* g, struct AstNode* ast) {
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

void gen_int_expr(struct CodeGen* g, struct AstNode* ast) {
    printf("  push %d\n", ast->node_int_value);
}

void gen_str_expr(struct CodeGen* g, struct AstNode* ast) {
    printf("  mov rax, OFFSET FLAG:.Lstr__%d\n", ast->node_idx);
    printf("  push rax\n");
}

void gen_unary_expr(struct CodeGen* g, struct AstNode* ast) {
    gen_expr(g, ast->node_operand, GEN_RVAL);
    if (ast->node_op == TK_NOT) {
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

void gen_ref_expr(struct CodeGen* g, struct AstNode* ast, int gen_mode) {
    gen_expr(g, ast->node_operand, GEN_LVAL);
}

void gen_lval2rval(struct Type* ty) {
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

void gen_deref_expr(struct CodeGen* g, struct AstNode* ast, int gen_mode) {
    gen_expr(g, ast->node_operand, GEN_RVAL);
    if (gen_mode == GEN_RVAL) {
        gen_lval2rval(ast->node_operand->ty->to);
    }
}

void gen_logical_expr(struct CodeGen* g, struct AstNode* ast) {
    int label = gen_new_label(g);

    if (ast->node_op == TK_ANDAND) {
        gen_expr(g, ast->node_lhs, GEN_RVAL);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lelse%d\n", label);
        gen_expr(g, ast->node_rhs, GEN_RVAL);
        printf("  jmp .Lend%d\n", label);
        printf(".Lelse%d:\n", label);
        printf("  push 0\n");
        printf(".Lend%d:\n", label);
    } else {
        gen_expr(g, ast->node_lhs, GEN_RVAL);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lelse%d\n", label);
        printf("  push 1\n");
        printf("  jmp .Lend%d\n", label);
        printf(".Lelse%d:\n", label);
        gen_expr(g, ast->node_rhs, GEN_RVAL);
        printf(".Lend%d:\n", label);
    }
}

void gen_binary_expr(struct CodeGen* g, struct AstNode* ast, int gen_mode) {
    gen_expr(g, ast->node_lhs, gen_mode);
    gen_expr(g, ast->node_rhs, gen_mode);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    if (ast->node_op == TK_PLUS) {
        printf("  add rax, rdi\n");
    } else if (ast->node_op == TK_MINUS) {
        printf("  sub rax, rdi\n");
    } else if (ast->node_op == TK_STAR) {
        printf("  imul rax, rdi\n");
    } else if (ast->node_op == TK_SLASH) {
        printf("  cqo\n");
        printf("  idiv rdi\n");
    } else if (ast->node_op == TK_PERCENT) {
        printf("  cqo\n");
        printf("  idiv rdi\n");
        printf("  mov rax, rdx\n");
    } else if (ast->node_op == TK_EQ) {
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
    } else if (ast->node_op == TK_NE) {
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
    } else if (ast->node_op == TK_LT) {
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
    } else if (ast->node_op == TK_LE) {
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
    } else {
        unreachable();
    }
    printf("  push rax\n");
}

void gen_assign_expr(struct CodeGen* g, struct AstNode* ast) {
    gen_expr(g, ast->node_lhs, GEN_LVAL);
    gen_expr(g, ast->node_rhs, GEN_RVAL);
    if (ast->node_op == TK_ASSIGN) {
    } else if (ast->node_op == TK_ASSIGN_ADD) {
        printf("  pop rdi\n");
        printf("  push [rsp]\n");
        gen_lval2rval(ast->node_lhs->ty);
        printf("  pop rax\n");
        printf("  add rax, rdi\n");
        printf("  push rax\n");
    } else if (ast->node_op == TK_ASSIGN_SUB) {
        printf("  pop rdi\n");
        printf("  push [rsp]\n");
        gen_lval2rval(ast->node_lhs->ty);
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

void gen_func_call(struct CodeGen* g, struct AstNode* ast) {
    char* func_name = ast->name;
    struct AstNode* args = ast->node_args;
    int i;
    for (i = 0; i < args->node_len; ++i) {
        struct AstNode* arg = args->node_items + i;
        gen_expr(g, arg, GEN_RVAL);
    }
    for (i = args->node_len - 1; i >= 0; --i) {
        printf("  pop %s\n", param_reg(i));
    }

    int label = gen_new_label(g);

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

void gen_lvar(struct CodeGen* g, struct AstNode* ast, int gen_mode) {
    int offset = 8 + ast->node_idx * 8;
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", offset);
    printf("  push rax\n");
    if (gen_mode == GEN_RVAL) {
        gen_lval2rval(ast->ty);
    }
}

void gen_expr(struct CodeGen* g, struct AstNode* ast, int gen_mode) {
    if (ast->kind == AST_INT_EXPR) {
        gen_int_expr(g, ast);
    } else if (ast->kind == AST_STR_EXPR) {
        gen_str_expr(g, ast);
    } else if (ast->kind == AST_UNARY_EXPR) {
        gen_unary_expr(g, ast);
    } else if (ast->kind == AST_REF_EXPR) {
        gen_ref_expr(g, ast, gen_mode);
    } else if (ast->kind == AST_DEREF_EXPR) {
        gen_deref_expr(g, ast, gen_mode);
    } else if (ast->kind == AST_BINARY_EXPR) {
        gen_binary_expr(g, ast, gen_mode);
    } else if (ast->kind == AST_LOGICAL_EXPR) {
        gen_logical_expr(g, ast);
    } else if (ast->kind == AST_ASSIGN_EXPR) {
        gen_assign_expr(g, ast);
    } else if (ast->kind == AST_FUNC_CALL) {
        gen_func_call(g, ast);
    } else if (ast->kind == AST_LVAR) {
        gen_lvar(g, ast, gen_mode);
    } else {
        unreachable();
    }
}

void gen_return_stmt(struct CodeGen* g, struct AstNode* ast) {
    if (ast->node_expr) {
        gen_expr(g, ast->node_expr, GEN_RVAL);
        printf("  pop rax\n");
    }
    gen_func_epilogue(g, ast);
}

void gen_if_stmt(struct CodeGen* g, struct AstNode* ast) {
    int label = gen_new_label(g);

    gen_expr(g, ast->node_cond, GEN_RVAL);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%d\n", label);
    gen_stmt(g, ast->node_then);
    printf("  jmp .Lend%d\n", label);
    printf(".Lelse%d:\n", label);
    if (ast->node_else) {
        gen_stmt(g, ast->node_else);
    }
    printf(".Lend%d:\n", label);
}

void gen_for_stmt(struct CodeGen* g, struct AstNode* ast) {
    int label = gen_new_label(g);
    ++g->loop_labels;
    *g->loop_labels = label;

    if (ast->node_init) {
        gen_expr(g, ast->node_init, GEN_RVAL);
        printf("  pop rax\n");
    }
    printf(".Lbegin%d:\n", label);
    gen_expr(g, ast->node_cond, GEN_RVAL);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", label);
    gen_stmt(g, ast->node_body);
    printf(".Lcontinue%d:\n", label);
    if (ast->node_update) {
        gen_expr(g, ast->node_update, GEN_RVAL);
        printf("  pop rax\n");
    }
    printf("  jmp .Lbegin%d\n", label);
    printf(".Lend%d:\n", label);

    --g->loop_labels;
}

void gen_break_stmt(struct CodeGen* g, struct AstNode* ast) {
    int label = *g->loop_labels;
    printf("  jmp .Lend%d\n", label);
}

void gen_continue_stmt(struct CodeGen* g, struct AstNode* ast) {
    int label = *g->loop_labels;
    printf("  jmp .Lcontinue%d\n", label);
}

void gen_expr_stmt(struct CodeGen* g, struct AstNode* ast) {
    gen_expr(g, ast->node_expr, GEN_RVAL);
    printf("  pop rax\n");
}

void gen_var_decl(struct CodeGen* g, struct AstNode* ast) {
}

void gen_block_stmt(struct CodeGen* g, struct AstNode* ast) {
    int i;
    for (i = 0; i < ast->node_len; ++i) {
        struct AstNode* stmt = ast->node_items + i;
        gen_stmt(g, stmt);
    }
}

void gen_stmt(struct CodeGen* g, struct AstNode* ast) {
    if (ast->kind == AST_LIST) {
        gen_block_stmt(g, ast);
    } else if (ast->kind == AST_RETURN_STMT) {
        gen_return_stmt(g, ast);
    } else if (ast->kind == AST_IF_STMT) {
        gen_if_stmt(g, ast);
    } else if (ast->kind == AST_FOR_STMT) {
        gen_for_stmt(g, ast);
    } else if (ast->kind == AST_BREAK_STMT) {
        gen_break_stmt(g, ast);
    } else if (ast->kind == AST_CONTINUE_STMT) {
        gen_continue_stmt(g, ast);
    } else if (ast->kind == AST_EXPR_STMT) {
        gen_expr_stmt(g, ast);
    } else if (ast->kind == AST_VAR_DECL) {
        gen_var_decl(g, ast);
    } else {
        unreachable();
    }
}

void gen_func(struct CodeGen* g, struct AstNode* ast) {
    printf("%s:\n", ast->name);

    gen_func_prologue(g, ast);
    gen_stmt(g, ast->node_body);
    gen_func_epilogue(g, ast);

    printf("\n");
}

void gen(struct CodeGen* g, struct Program* prog) {
    printf(".intel_syntax noprefix\n\n");

    int i;
    for (i = 0; prog->str_literals[i]; ++i) {
        printf(".Lstr__%d:\n", i + 1);
        printf("  .string \"%s\"\n\n", prog->str_literals[i]);
    }

    printf(".globl main\n\n");

    for (i = 0; i < prog->funcs->node_len; ++i) {
        struct AstNode* func = prog->funcs->node_items + i;
        gen_func(g, func);
    }
}

int main() {
    char* source = calloc(1024*1024, sizeof(char));
    read_all(source);
    struct Token* tokens = tokenize(source);

    struct Parser* parser = parser_new(tokens);
    struct Program* prog = parse(parser);

    struct CodeGen* codegen = codegen_new();
    gen(codegen, prog);

    return 0;
}
