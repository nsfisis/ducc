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
    TokenKind_ellipsis,
    TokenKind_eq,
    TokenKind_ge,
    TokenKind_gt,
    TokenKind_ident,
    TokenKind_keyword_break,
    TokenKind_keyword_char,
    TokenKind_keyword_const,
    TokenKind_keyword_continue,
    TokenKind_keyword_do,
    TokenKind_keyword_else,
    TokenKind_keyword_enum,
    TokenKind_keyword_extern,
    TokenKind_keyword_for,
    TokenKind_keyword_if,
    TokenKind_keyword_int,
    TokenKind_keyword_long,
    TokenKind_keyword_return,
    TokenKind_keyword_sizeof,
    TokenKind_keyword_struct,
    TokenKind_keyword_typeof,
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
    TokenKind_or,
    TokenKind_oror,
    TokenKind_paren_l,
    TokenKind_paren_r,
    TokenKind_percent,
    TokenKind_plus,
    TokenKind_plusplus,
    TokenKind_semicolon,
    TokenKind_slash,
    TokenKind_star,

    // va_start() is currently implemented as a special form due to the current limitation of #define macro.
    TokenKind_va_start,
};
typedef enum TokenKind TokenKind;

struct Token {
    TokenKind kind;
    String raw;
};
typedef struct Token Token;

const char* token_kind_stringify(TokenKind k) {
    if (k == TokenKind_eof)
        return "<eof>";
    else if (k == TokenKind_and)
        return "&";
    else if (k == TokenKind_andand)
        return "&&";
    else if (k == TokenKind_arrow)
        return "->";
    else if (k == TokenKind_assign)
        return "=";
    else if (k == TokenKind_assign_add)
        return "+=";
    else if (k == TokenKind_assign_sub)
        return "-=";
    else if (k == TokenKind_brace_l)
        return "{";
    else if (k == TokenKind_brace_r)
        return "}";
    else if (k == TokenKind_bracket_l)
        return "[";
    else if (k == TokenKind_bracket_r)
        return "]";
    else if (k == TokenKind_comma)
        return ",";
    else if (k == TokenKind_dot)
        return ".";
    else if (k == TokenKind_ellipsis)
        return "...";
    else if (k == TokenKind_eq)
        return "==";
    else if (k == TokenKind_ge)
        return ">=";
    else if (k == TokenKind_gt)
        return ">";
    else if (k == TokenKind_ident)
        return "<identifier>";
    else if (k == TokenKind_keyword_break)
        return "break";
    else if (k == TokenKind_keyword_char)
        return "char";
    else if (k == TokenKind_keyword_const)
        return "const";
    else if (k == TokenKind_keyword_continue)
        return "continue";
    else if (k == TokenKind_keyword_do)
        return "do";
    else if (k == TokenKind_keyword_else)
        return "else";
    else if (k == TokenKind_keyword_enum)
        return "enum";
    else if (k == TokenKind_keyword_extern)
        return "extern";
    else if (k == TokenKind_keyword_for)
        return "for";
    else if (k == TokenKind_keyword_if)
        return "if";
    else if (k == TokenKind_keyword_int)
        return "int";
    else if (k == TokenKind_keyword_long)
        return "long";
    else if (k == TokenKind_keyword_return)
        return "return";
    else if (k == TokenKind_keyword_sizeof)
        return "sizeof";
    else if (k == TokenKind_keyword_struct)
        return "struct";
    else if (k == TokenKind_keyword_typeof)
        return "typeof";
    else if (k == TokenKind_keyword_void)
        return "void";
    else if (k == TokenKind_keyword_while)
        return "while";
    else if (k == TokenKind_le)
        return "le";
    else if (k == TokenKind_lt)
        return "lt";
    else if (k == TokenKind_literal_int)
        return "<integer>";
    else if (k == TokenKind_literal_str)
        return "<string>";
    else if (k == TokenKind_minus)
        return "-";
    else if (k == TokenKind_minusminus)
        return "--";
    else if (k == TokenKind_ne)
        return "!=";
    else if (k == TokenKind_not)
        return "!";
    else if (k == TokenKind_or)
        return "|";
    else if (k == TokenKind_oror)
        return "||";
    else if (k == TokenKind_paren_l)
        return "(";
    else if (k == TokenKind_paren_r)
        return ")";
    else if (k == TokenKind_percent)
        return "%";
    else if (k == TokenKind_plus)
        return "+";
    else if (k == TokenKind_plusplus)
        return "++";
    else if (k == TokenKind_semicolon)
        return ";";
    else if (k == TokenKind_slash)
        return "/";
    else if (k == TokenKind_star)
        return "*";
    else if (k == TokenKind_va_start)
        return "va_start";
    else
        unreachable();
}

const char* token_stringify(Token* t) {
    TokenKind k = t->kind;
    if (k == TokenKind_ident || k == TokenKind_literal_int || k == TokenKind_literal_str) {
        const char* kind_str = token_kind_stringify(k);
        char* buf = calloc(t->raw.len + strlen(kind_str) + 3 + 1, sizeof(char));
        sprintf(buf, "%.*s (%s)", t->raw.len, t->raw.data, kind_str);
        return buf;
    } else {
        return token_kind_stringify(k);
    }
}

struct Lexer {
    PpToken* src;
    int pos;
    Token* tokens;
    int n_tokens;
};
typedef struct Lexer Lexer;

Lexer* lexer_new(PpToken* pp_tokens) {
    Lexer* l = calloc(1, sizeof(Lexer));
    l->src = pp_tokens;
    l->tokens = calloc(1024 * 1024, sizeof(Token));
    return l;
}

void tokenize_all(Lexer* l) {
    int ch;
    int start;
    while (l->src[l->pos].kind != PpTokenKind_eof) {
        PpToken* pp_tok = l->src + l->pos;
        Token* tok = l->tokens + l->n_tokens;
        PpTokenKind k = pp_tok->kind;
        ++l->pos;
        if (k == PpTokenKind_header_name) {
            unimplemented();
        } else if (k == PpTokenKind_identifier) {
            if (string_equals_cstr(&pp_tok->raw, "break")) {
                tok->kind = TokenKind_keyword_break;
            } else if (string_equals_cstr(&pp_tok->raw, "char")) {
                tok->kind = TokenKind_keyword_char;
            } else if (string_equals_cstr(&pp_tok->raw, "const")) {
                tok->kind = TokenKind_keyword_const;
            } else if (string_equals_cstr(&pp_tok->raw, "continue")) {
                tok->kind = TokenKind_keyword_continue;
            } else if (string_equals_cstr(&pp_tok->raw, "do")) {
                tok->kind = TokenKind_keyword_do;
            } else if (string_equals_cstr(&pp_tok->raw, "else")) {
                tok->kind = TokenKind_keyword_else;
            } else if (string_equals_cstr(&pp_tok->raw, "enum")) {
                tok->kind = TokenKind_keyword_enum;
            } else if (string_equals_cstr(&pp_tok->raw, "extern")) {
                tok->kind = TokenKind_keyword_extern;
            } else if (string_equals_cstr(&pp_tok->raw, "for")) {
                tok->kind = TokenKind_keyword_for;
            } else if (string_equals_cstr(&pp_tok->raw, "if")) {
                tok->kind = TokenKind_keyword_if;
            } else if (string_equals_cstr(&pp_tok->raw, "int")) {
                tok->kind = TokenKind_keyword_int;
            } else if (string_equals_cstr(&pp_tok->raw, "long")) {
                tok->kind = TokenKind_keyword_long;
            } else if (string_equals_cstr(&pp_tok->raw, "return")) {
                tok->kind = TokenKind_keyword_return;
            } else if (string_equals_cstr(&pp_tok->raw, "sizeof")) {
                tok->kind = TokenKind_keyword_sizeof;
            } else if (string_equals_cstr(&pp_tok->raw, "struct")) {
                tok->kind = TokenKind_keyword_struct;
            } else if (string_equals_cstr(&pp_tok->raw, "typedef")) {
                tok->kind = TokenKind_keyword_typeof;
            } else if (string_equals_cstr(&pp_tok->raw, "void")) {
                tok->kind = TokenKind_keyword_void;
            } else if (string_equals_cstr(&pp_tok->raw, "while")) {
                tok->kind = TokenKind_keyword_while;
            } else if (string_equals_cstr(&pp_tok->raw, "va_start")) {
                tok->kind = TokenKind_va_start;
            } else {
                tok->kind = TokenKind_ident;
            }
            tok->raw = pp_tok->raw;
        } else if (k == PpTokenKind_pp_number) {
            tok->kind = TokenKind_literal_int;
            tok->raw = pp_tok->raw;
        } else if (k == PpTokenKind_character_constant) {
            tok->kind = TokenKind_literal_int;
            ch = pp_tok->raw.data[1];
            if (ch == '\\') {
                ch = pp_tok->raw.data[2];
                if (ch == 'a') {
                    ch = '\a';
                } else if (ch == 'b') {
                    ch = '\b';
                } else if (ch == 'f') {
                    ch = '\f';
                } else if (ch == 'n') {
                    ch = '\n';
                } else if (ch == 'r') {
                    ch = '\r';
                } else if (ch == 't') {
                    ch = '\t';
                } else if (ch == 'v') {
                    ch = '\v';
                } else if (ch == '0') {
                    ch = '\0';
                }
            }
            char* buf = calloc(4, sizeof(char));
            sprintf(buf, "%d", ch);
            tok->raw.data = buf;
            tok->raw.len = strlen(buf);
        } else if (k == PpTokenKind_string_literal) {
            tok->kind = TokenKind_literal_str;
            tok->raw.data = pp_tok->raw.data + 1;
            tok->raw.len = pp_tok->raw.len - 2;
        } else if (k == PpTokenKind_punctuator || k == PpTokenKind_other) {
            if (string_equals_cstr(&pp_tok->raw, "(")) {
                tok->kind = TokenKind_paren_l;
            } else if (string_equals_cstr(&pp_tok->raw, ")")) {
                tok->kind = TokenKind_paren_r;
            } else if (string_equals_cstr(&pp_tok->raw, "{")) {
                tok->kind = TokenKind_brace_l;
            } else if (string_equals_cstr(&pp_tok->raw, "}")) {
                tok->kind = TokenKind_brace_r;
            } else if (string_equals_cstr(&pp_tok->raw, "[")) {
                tok->kind = TokenKind_bracket_l;
            } else if (string_equals_cstr(&pp_tok->raw, "]")) {
                tok->kind = TokenKind_bracket_r;
            } else if (string_equals_cstr(&pp_tok->raw, ",")) {
                tok->kind = TokenKind_comma;
            } else if (string_equals_cstr(&pp_tok->raw, ";")) {
                tok->kind = TokenKind_semicolon;
            } else if (string_equals_cstr(&pp_tok->raw, "+=")) {
                tok->kind = TokenKind_assign_add;
            } else if (string_equals_cstr(&pp_tok->raw, "++")) {
                tok->kind = TokenKind_plusplus;
            } else if (string_equals_cstr(&pp_tok->raw, "+")) {
                tok->kind = TokenKind_plus;
            } else if (string_equals_cstr(&pp_tok->raw, "||")) {
                tok->kind = TokenKind_oror;
            } else if (string_equals_cstr(&pp_tok->raw, "|")) {
                tok->kind = TokenKind_or;
            } else if (string_equals_cstr(&pp_tok->raw, "&&")) {
                tok->kind = TokenKind_andand;
            } else if (string_equals_cstr(&pp_tok->raw, "&")) {
                tok->kind = TokenKind_and;
            } else if (string_equals_cstr(&pp_tok->raw, "->")) {
                tok->kind = TokenKind_arrow;
            } else if (string_equals_cstr(&pp_tok->raw, "-=")) {
                tok->kind = TokenKind_assign_sub;
            } else if (string_equals_cstr(&pp_tok->raw, "--")) {
                tok->kind = TokenKind_minusminus;
            } else if (string_equals_cstr(&pp_tok->raw, "-")) {
                tok->kind = TokenKind_minus;
            } else if (string_equals_cstr(&pp_tok->raw, "*")) {
                tok->kind = TokenKind_star;
            } else if (string_equals_cstr(&pp_tok->raw, "/")) {
                tok->kind = TokenKind_slash;
            } else if (string_equals_cstr(&pp_tok->raw, "%")) {
                tok->kind = TokenKind_percent;
            } else if (string_equals_cstr(&pp_tok->raw, "...")) {
                tok->kind = TokenKind_ellipsis;
            } else if (string_equals_cstr(&pp_tok->raw, ".")) {
                tok->kind = TokenKind_dot;
            } else if (string_equals_cstr(&pp_tok->raw, "!=")) {
                tok->kind = TokenKind_ne;
            } else if (string_equals_cstr(&pp_tok->raw, "!")) {
                tok->kind = TokenKind_not;
            } else if (string_equals_cstr(&pp_tok->raw, "==")) {
                tok->kind = TokenKind_eq;
            } else if (string_equals_cstr(&pp_tok->raw, "=")) {
                tok->kind = TokenKind_assign;
            } else if (string_equals_cstr(&pp_tok->raw, "<=")) {
                tok->kind = TokenKind_le;
            } else if (string_equals_cstr(&pp_tok->raw, "<")) {
                tok->kind = TokenKind_lt;
            } else if (string_equals_cstr(&pp_tok->raw, ">=")) {
                tok->kind = TokenKind_ge;
            } else if (string_equals_cstr(&pp_tok->raw, ">")) {
                tok->kind = TokenKind_gt;
            } else {
                fatal_error("unknown token: %.*s", pp_tok->raw.len, pp_tok->raw.data);
            }
            tok->raw = pp_tok->raw;
        } else if (k == PpTokenKind_whitespace) {
            continue;
        }
        ++l->n_tokens;
    }
}

Token* tokenize(PpToken* pp_tokens) {
    Lexer* l = lexer_new(pp_tokens);
    tokenize_all(l);
    return l->tokens;
}
