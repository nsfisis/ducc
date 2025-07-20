typedef long size_t;

struct FILE;
typedef struct FILE FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int atoi(const char*);
void* calloc(size_t, size_t);
void exit(int);
int fclose(FILE*);
int fprintf(FILE*, const char*, ...);
char* fgets(char*, int, FILE*);
FILE* fopen(const char*, const char*);
int getchar(void);
int isalnum(int);
int isalpha(int);
int isdigit(int);
int isspace(int);
void* memcpy(void*, void*, size_t);
void* memmove(void*, void*, size_t);
int printf(const char*, ...);
int sprintf(char*, const char*, ...);
int strcmp(const char*, const char*);
size_t strlen(const char*);
int strncmp(const char*, const char*, size_t);
char* strstr(const char*, const char*);

#define NULL 0

void fatal_error(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void unreachable() {
    fatal_error("unreachable");
}

char* read_all(FILE* in) {
    char* buf = calloc(1024 * 1024, sizeof(char));
    char* cur = buf;
    char* tmp = calloc(1024, sizeof(char));
    while (fgets(tmp, 1024, in)) {
        size_t len = strlen(tmp);
        memcpy(cur, tmp, len);
        cur += len;
    }
    return buf;
}

struct String {
    char* data;
    size_t len;
};
typedef struct String String;

char* string_to_cstr(const String* s) {
    char* buf = calloc(s->len + 1, sizeof(char));
    memcpy(buf, s->data, s->len);
    return buf;
}

int string_equals(const String* s1, const String* s2) {
    return s1->len == s2->len && strncmp(s1->data, s2->data, s1->len) == 0;
}

int string_equals_cstr(const String* s1, const char* s2) {
    size_t s2_len = strlen(s2);
    return s1->len == s2_len && strncmp(s1->data, s2, s1->len) == 0;
}

enum PpTokenKind {
    PpTokenKind_eof,

    PpTokenKind_header_name,
    PpTokenKind_identifier,
    PpTokenKind_pp_number,
    PpTokenKind_character_constant,
    PpTokenKind_string_literal,
    PpTokenKind_punctuator,
    PpTokenKind_other,
    PpTokenKind_whitespace,
};
typedef enum PpTokenKind PpTokenKind;

struct PpToken {
    PpTokenKind kind;
    String raw;
};
typedef struct PpToken PpToken;

struct PpDefine {
    String name;
    PpToken* tokens;
};
typedef struct PpDefine PpDefine;

struct Preprocessor {
    char* src;
    int pos;
    PpToken* pp_tokens;
    int n_pp_tokens;
    PpDefine* pp_defines;
    int n_pp_defines;
    int include_depth;
};
typedef struct Preprocessor Preprocessor;

PpToken* do_preprocess(char* src, int depth);

Preprocessor* preprocessor_new(char* src, int include_depth) {
    if (include_depth >= 32) {
        fatal_error("include depth limit exceeded");
    }

    Preprocessor* pp = calloc(1, sizeof(Preprocessor));
    pp->src = src;
    pp->pp_tokens = calloc(1024 * 1024, sizeof(PpToken));
    pp->pp_defines = calloc(1024, sizeof(PpDefine));
    pp->include_depth = include_depth;

    return pp;
}

int find_pp_define(Preprocessor* pp, String* name) {
    int i;
    for (i = 0; i < pp->n_pp_defines; ++i) {
        if (string_equals(&pp->pp_defines[i].name, name)) {
            return i;
        }
    }
    return -1;
}

void pp_tokenize_all(Preprocessor* pp) {
    char* buf;
    int ch;
    int start;
    while (pp->src[pp->pos]) {
        PpToken* tok = pp->pp_tokens + pp->n_pp_tokens;
        char c = pp->src[pp->pos];
        ++pp->pos;
        if (c == '(') {
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == ')') {
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == '{') {
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == '}') {
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == '[') {
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == ']') {
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == ',') {
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == ';') {
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == '+') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else if (pp->src[pp->pos] == '+') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '|') {
            ++pp->pos;
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 2;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == '&') {
            if (pp->src[pp->pos] == '&') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '-') {
            if (pp->src[pp->pos] == '>') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else if (pp->src[pp->pos] == '-') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '*') {
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == '/') {
            if (pp->src[pp->pos] == '/') {
                start = pp->pos - 1;
                ++pp->pos;
                while (pp->src[pp->pos] && pp->src[pp->pos] != '\n' && pp->src[pp->pos] != '\r') {
                    ++pp->pos;
                }
                tok->kind = PpTokenKind_whitespace;
                tok->raw.len = pp->pos - start;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else if (pp->src[pp->pos] == '*') {
                fatal_error("unimplemented");
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '%') {
            tok->kind = PpTokenKind_punctuator;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else if (c == '.') {
            if (pp->src[pp->pos] == '.') {
                ++pp->pos;
                if (pp->src[pp->pos] == '.') {
                    ++pp->pos;
                    tok->kind = PpTokenKind_punctuator;
                    tok->raw.len = 3;
                    tok->raw.data = pp->src + pp->pos - tok->raw.len;
                } else {
                    --pp->pos;
                    tok->kind = PpTokenKind_punctuator;
                    tok->raw.len = 1;
                    tok->raw.data = pp->src + pp->pos - tok->raw.len;
                }
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '!') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '=') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '<') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '>') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '#') {
            if (pp->src[pp->pos] == '#') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '\'') {
            start = pp->pos - 1;
            if (pp->src[pp->pos] == '\\') {
                ++pp->pos;
            }
            pp->pos += 2;
            tok->kind = PpTokenKind_character_constant;
            tok->raw.data = pp->src + start;
            tok->raw.len = pp->pos - start;
        } else if (c == '"') {
            start = pp->pos - 1;
            while (1) {
                ch = pp->src[pp->pos];
                if (ch == '\\') {
                    ++pp->pos;
                } else if (ch == '"') {
                    break;
                }
                ++pp->pos;
            }
            ++pp->pos;
            tok->kind = PpTokenKind_string_literal;
            tok->raw.data = pp->src + start;
            tok->raw.len = pp->pos - start;
        } else if (isdigit(c)) {
            --pp->pos;
            start = pp->pos;
            while (isdigit(pp->src[pp->pos])) {
                ++pp->pos;
            }
            tok->kind = PpTokenKind_pp_number;
            tok->raw.data = pp->src + start;
            tok->raw.len = pp->pos - start;
        } else if (isalpha(c) || c == '_') {
            --pp->pos;
            start = pp->pos;
            while (isalnum(pp->src[pp->pos]) || pp->src[pp->pos] == '_') {
                ++pp->pos;
            }
            tok->raw.data = pp->src + start;
            tok->raw.len = pp->pos - start;
            tok->kind = PpTokenKind_identifier;
        } else if (isspace(c)) {
            tok->raw.data = pp->src;
            tok->raw.len = 1;
            tok->kind = PpTokenKind_whitespace;
        } else {
            tok->raw.data = pp->src;
            tok->raw.len = 1;
            tok->kind = PpTokenKind_other;
        }
        ++pp->n_pp_tokens;
    }
}

void process_pp_directives(Preprocessor* pp) {
    PpToken* tok = pp->pp_tokens;
    PpToken* define_dest;
    while (tok->kind != PpTokenKind_eof) {
        // TODO: check if the token is at the beginning of line.
        // TODO: check if skipped whitespaces do not contain line breaks.
        if (tok->kind == PpTokenKind_punctuator && string_equals_cstr(&tok->raw, "#")) {
            PpToken* tok2 = tok + 1;
            while (tok2->kind != PpTokenKind_eof && tok2->kind == PpTokenKind_whitespace)
                ++tok2;
            if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "define")) {
                ++tok2;
                while (tok2->kind != PpTokenKind_eof && tok2->kind == PpTokenKind_whitespace)
                    ++tok2;
                if (tok2->kind == PpTokenKind_identifier) {
                    PpToken* define_name = tok2;
                    ++tok2;
                    while (tok2->kind != PpTokenKind_eof && tok2->kind == PpTokenKind_whitespace)
                        ++tok2;
                    if (tok2->kind == PpTokenKind_identifier || tok2->kind == PpTokenKind_pp_number) {
                        define_dest = tok2;

                        pp->pp_defines[pp->n_pp_defines].name.len = define_name->raw.len;
                        pp->pp_defines[pp->n_pp_defines].name.data = define_name->raw.data;
                        pp->pp_defines[pp->n_pp_defines].tokens = calloc(1, sizeof(PpToken));
                        pp->pp_defines[pp->n_pp_defines].tokens[0].kind = define_dest->kind;
                        pp->pp_defines[pp->n_pp_defines].tokens[0].raw.len = define_dest->raw.len;
                        pp->pp_defines[pp->n_pp_defines].tokens[0].raw.data = define_dest->raw.data;
                        ++pp->n_pp_defines;
                    }
                }
                // Remove #define directive.
                while (tok != tok2 + 1) {
                    tok->kind = PpTokenKind_whitespace;
                    tok->raw.len = 0;
                    tok->raw.data = NULL;
                    ++tok;
                }
            } else if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "include")) {
                ++tok2;
                while (tok2->kind != PpTokenKind_eof && tok2->kind == PpTokenKind_whitespace)
                    ++tok2;
                if (tok2->kind == PpTokenKind_string_literal) {
                    // Process #include directive.
                    PpToken* include_name = tok2;
                    ++tok2;
                    char* include_name_buf = calloc(include_name->raw.len - 2 + 1, sizeof(char));
                    sprintf(include_name_buf, "%.*s", include_name->raw.len - 2, include_name->raw.data + 1);

                    // Remove #include directive itself.
                    while (tok != tok2 + 1) {
                        tok->kind = PpTokenKind_whitespace;
                        tok->raw.len = 0;
                        tok->raw.data = NULL;
                        ++tok;
                    }

                    // Read and preprocess included file.
                    FILE* include_file = fopen(include_name_buf, "rb");
                    if (include_file == NULL) {
                        char* buf = calloc(1024, sizeof(char));
                        sprintf(buf, "cannot open include file: %s", include_name_buf);
                        fatal_error(buf);
                    }
                    char* include_source = read_all(include_file);
                    fclose(include_file);

                    PpToken* include_pp_tokens = do_preprocess(include_source, pp->include_depth + 1);

                    // Insert preprocessed tokens into the current token stream.
                    int n_include_pp_tokens = 0;
                    while (include_pp_tokens[n_include_pp_tokens].kind != PpTokenKind_eof) {
                        ++n_include_pp_tokens;
                    }

                    // Shift existing tokens to make room for include tokens
                    int n_pp_tokens_after_include = pp->n_pp_tokens - (tok - pp->pp_tokens);

                    memmove(tok + n_include_pp_tokens, tok, n_pp_tokens_after_include * sizeof(PpToken));

                    // Copy include tokens into the current position
                    memcpy(tok, include_pp_tokens, n_include_pp_tokens * sizeof(PpToken));

                    pp->n_pp_tokens += n_include_pp_tokens;
                }
            }
        } else if (tok->kind == PpTokenKind_identifier) {
            int pp_define_idx = find_pp_define(pp, &tok->raw);
            if (pp_define_idx != -1) {
                define_dest = pp->pp_defines[pp_define_idx].tokens;
                tok->kind = define_dest->kind;
                tok->raw.data = define_dest->raw.data;
                tok->raw.len = define_dest->raw.len;
            }
        }
        ++tok;
    }
}

PpToken* do_preprocess(char* src, int depth) {
    Preprocessor* pp = preprocessor_new(src, depth);
    pp_tokenize_all(pp);
    process_pp_directives(pp);
    return pp->pp_tokens;
}

PpToken* preprocess(char* src) {
    return do_preprocess(src, 0);
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
    else
        unreachable();
}

const char* token_stringify(Token* t) {
    TokenKind k = t->kind;
    if (k == TokenKind_ident || k == TokenKind_literal_int || k == TokenKind_literal_str) {
        char* buf = calloc(t->raw.len + 1, sizeof(char));
        sprintf(buf, "%.*s (%s)", t->raw.len, t->raw.data, token_kind_stringify(k));
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
    char* buf;
    int ch;
    int start;
    while (l->src[l->pos].kind != PpTokenKind_eof) {
        PpToken* pp_tok = l->src + l->pos;
        Token* tok = l->tokens + l->n_tokens;
        PpTokenKind k = pp_tok->kind;
        ++l->pos;
        if (k == PpTokenKind_header_name) {
            fatal_error("not implemented yet");
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
            } else {
                tok->kind = TokenKind_ident;
            }
            tok->raw.data = pp_tok->raw.data;
            tok->raw.len = pp_tok->raw.len;
        } else if (k == PpTokenKind_pp_number) {
            tok->kind = TokenKind_literal_int;
            tok->raw.data = pp_tok->raw.data;
            tok->raw.len = pp_tok->raw.len;
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
                }
            }
            buf = calloc(4, sizeof(char));
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
                sprintf(buf, "unknown token: %.*s", pp_tok->raw.len, pp_tok->raw.data);
                fatal_error(buf);
            }
            tok->raw.data = pp_tok->raw.data;
            tok->raw.len = pp_tok->raw.len;
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
typedef enum TypeKind TypeKind;

struct AstNode;

struct Type {
    TypeKind kind;
    struct Type* to;
    struct AstNode* def;
};
typedef struct Type Type;

Type* type_new(TypeKind kind) {
    Type* ty = calloc(1, sizeof(Type));
    ty->kind = kind;
    return ty;
}

Type* type_new_ptr(Type* to) {
    Type* ty = calloc(1, sizeof(Type));
    ty->kind = TypeKind_ptr;
    ty->to = to;
    return ty;
}

int type_is_unsized(Type* ty) {
    return ty->kind != TypeKind_void;
}

int type_sizeof_struct(Type* ty);
int type_alignof_struct(Type* ty);
int type_offsetof(Type* ty, const String* name);
Type* type_member_typeof(Type* ty, const String* name);

int type_sizeof(Type* ty) {
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

int type_alignof(Type* ty) {
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
    AstNodeKind_do_while_stmt,
    AstNodeKind_enum_def,
    AstNodeKind_enum_member,
    AstNodeKind_expr_stmt,
    AstNodeKind_for_stmt,
    AstNodeKind_func_call,
    AstNodeKind_func_decl,
    AstNodeKind_func_def,
    AstNodeKind_gvar,
    AstNodeKind_gvar_decl,
    AstNodeKind_if_stmt,
    AstNodeKind_int_expr,
    AstNodeKind_list,
    AstNodeKind_logical_expr,
    AstNodeKind_lvar,
    AstNodeKind_lvar_decl,
    AstNodeKind_param,
    AstNodeKind_ref_expr,
    AstNodeKind_return_stmt,
    AstNodeKind_str_expr,
    AstNodeKind_struct_decl,
    AstNodeKind_struct_def,
    AstNodeKind_struct_member,
    AstNodeKind_type,
    AstNodeKind_typedef_decl,
    AstNodeKind_unary_expr,
};
typedef enum AstNodeKind AstNodeKind;

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
    AstNodeKind kind;
    String name;
    Type* ty;
    struct AstNode* __n1;
    struct AstNode* __n2;
    struct AstNode* __n3;
    struct AstNode* __n4;
    int __i;
};
typedef struct AstNode AstNode;

struct Program {
    AstNode* funcs;
    char** str_literals;
};
typedef struct Program Program;

AstNode* ast_new(AstNodeKind kind) {
    AstNode* ast = calloc(1, sizeof(AstNode));
    ast->kind = kind;
    return ast;
}

AstNode* ast_new_list(int capacity) {
    AstNode* list = ast_new(AstNodeKind_list);
    list->node_items = calloc(capacity, sizeof(AstNode));
    list->node_len = 0;
    return list;
}

void ast_append(AstNode* list, AstNode* item) {
    if (list->kind != AstNodeKind_list) {
        fatal_error("ast_append: ast is not a list");
    }
    if (!item) {
        return;
    }
    memcpy(list->node_items + list->node_len, item, sizeof(AstNode));
    ++list->node_len;
}

AstNode* ast_new_int(int v) {
    AstNode* e = ast_new(AstNodeKind_int_expr);
    e->node_int_value = v;
    e->ty = type_new(TypeKind_int);
    return e;
}

AstNode* ast_new_unary_expr(int op, AstNode* operand) {
    AstNode* e = ast_new(AstNodeKind_unary_expr);
    e->node_op = op;
    e->node_operand = operand;
    e->ty = type_new(TypeKind_int);
    return e;
}

AstNode* ast_new_binary_expr(int op, AstNode* lhs, AstNode* rhs) {
    AstNode* e = ast_new(AstNodeKind_binary_expr);
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

AstNode* ast_new_assign_expr(int op, AstNode* lhs, AstNode* rhs) {
    AstNode* e = ast_new(AstNodeKind_assign_expr);
    e->node_op = op;
    e->node_lhs = lhs;
    e->node_rhs = rhs;
    e->ty = lhs->ty;
    return e;
}

AstNode* ast_new_assign_add_expr(AstNode* lhs, AstNode* rhs) {
    if (lhs->ty->kind == TypeKind_ptr) {
        rhs = ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->to)));
    } else if (rhs->ty->kind == TypeKind_ptr) {
        lhs = ast_new_binary_expr(TokenKind_star, lhs, ast_new_int(type_sizeof(rhs->ty->to)));
    }
    return ast_new_assign_expr(TokenKind_assign_add, lhs, rhs);
}

AstNode* ast_new_assign_sub_expr(AstNode* lhs, AstNode* rhs) {
    if (lhs->ty->kind == TypeKind_ptr) {
        rhs = ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->to)));
    }
    return ast_new_assign_expr(TokenKind_assign_sub, lhs, rhs);
}

AstNode* ast_new_ref_expr(AstNode* operand) {
    AstNode* e = ast_new(AstNodeKind_ref_expr);
    e->node_operand = operand;
    e->ty = type_new_ptr(operand->ty);
    return e;
}

AstNode* ast_new_deref_expr(AstNode* operand) {
    AstNode* e = ast_new(AstNodeKind_deref_expr);
    e->node_operand = operand;
    e->ty = operand->ty->to;
    return e;
}

AstNode* ast_new_member_access_expr(AstNode* obj, const String* name) {
    AstNode* e = ast_new(AstNodeKind_deref_expr);
    e->node_operand = ast_new_binary_expr(TokenKind_plus, obj, ast_new_int(type_offsetof(obj->ty->to, name)));
    e->ty = type_member_typeof(obj->ty->to, name);
    e->node_operand->ty = type_new_ptr(e->ty);
    return e;
}

int type_sizeof_struct(Type* ty) {
    int next_offset = 0;
    int struct_align = 0;
    int padding;

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        AstNode* member = ty->def->node_members->node_items + i;
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

int type_alignof_struct(Type* ty) {
    int struct_align = 0;

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        AstNode* member = ty->def->node_members->node_items + i;
        int align = type_alignof(member->ty);

        if (struct_align < align) {
            struct_align = align;
        }
    }
    return struct_align;
}

int type_offsetof(Type* ty, const String* name) {
    if (ty->kind != TypeKind_struct) {
        fatal_error("type_offsetof: type is not a struct");
    }

    int next_offset = 0;

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        AstNode* member = ty->def->node_members->node_items + i;
        int size = type_sizeof(member->ty);
        int align = type_alignof(member->ty);

        if (next_offset % align != 0) {
            int padding = align - next_offset % align;
            next_offset += padding;
        }
        if (string_equals(&member->name, name)) {
            return next_offset;
        }
        next_offset += size;
    }

    fatal_error("type_offsetof: member not found");
}

Type* type_member_typeof(Type* ty, const String* name) {
    if (ty->kind != TypeKind_struct) {
        fatal_error("type_offsetof: type is not a struct");
    }

    int i;
    for (i = 0; i < ty->def->node_members->node_len; ++i) {
        AstNode* member = ty->def->node_members->node_items + i;
        if (string_equals(&member->name, name)) {
            return member->ty;
        }
    }

    fatal_error("type_offsetof: member not found");
}

#define LVAR_MAX 32

struct LocalVar {
    String name;
    Type* ty;
};
typedef struct LocalVar LocalVar;

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
    Token* tokens;
    int pos;
    LocalVar* lvars;
    int n_lvars;
    GlobalVar* gvars;
    int n_gvars;
    Func* funcs;
    int n_funcs;
    AstNode* structs;
    int n_structs;
    AstNode* enums;
    int n_enums;
    AstNode* typedefs;
    int n_typedefs;
    char** str_literals;
    int n_str_literals;
};
typedef struct Parser Parser;

Parser* parser_new(Token* tokens) {
    Parser* p = calloc(1, sizeof(Parser));
    p->tokens = tokens;
    p->gvars = calloc(128, sizeof(GlobalVar));
    p->funcs = calloc(256, sizeof(Func));
    p->structs = calloc(64, sizeof(AstNode));
    p->enums = calloc(16, sizeof(AstNode));
    p->typedefs = calloc(64, sizeof(AstNode));
    p->str_literals = calloc(1024, sizeof(char*));
    return p;
}

Token* peek_token(Parser* p) {
    return p->tokens + p->pos;
}

Token* next_token(Parser* p) {
    ++p->pos;
    return p->tokens + p->pos - 1;
}

int eof(Parser* p) {
    return peek_token(p)->kind != TokenKind_eof;
}

Token* expect(Parser* p, int expected) {
    Token* t = next_token(p);
    if (t->kind == expected) {
        return t;
    }

    char* buf = calloc(1024, sizeof(char));
    sprintf(buf, "expected '%s', but got '%s'", token_kind_stringify(expected), token_stringify(t));
    fatal_error(buf);
}

int find_lvar(Parser* p, const String* name) {
    int i;
    for (i = 0; i < p->n_lvars; ++i) {
        if (string_equals(&p->lvars[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int find_gvar(Parser* p, const String* name) {
    int i;
    for (i = 0; i < p->n_gvars; ++i) {
        if (string_equals(&p->gvars[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int find_func(Parser* p, const String* name) {
    int i;
    for (i = 0; i < p->n_funcs; ++i) {
        if (string_equals(&p->funcs[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int find_struct(Parser* p, const String* name) {
    int i;
    for (i = 0; i < p->n_structs; ++i) {
        if (string_equals(&p->structs[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int find_enum(Parser* p, const String* name) {
    int i;
    for (i = 0; i < p->n_enums; ++i) {
        if (string_equals(&p->enums[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int find_enum_member(Parser* p, const String* name) {
    int i;
    int j;
    for (i = 0; i < p->n_enums; ++i) {
        for (j = 0; j < p->enums[i].node_members->node_len; ++j) {
            if (string_equals(&p->enums[i].node_members->node_items[j].name, name)) {
                return i * 1000 + j;
            }
        }
    }
    return -1;
}

int find_typedef(Parser* p, const String* name) {
    int i;
    for (i = 0; i < p->n_typedefs; ++i) {
        if (string_equals(&p->typedefs[i].name, name)) {
            return i;
        }
    }
    return -1;
}

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
    AstNode* e;
    char* buf;
    if (t->kind == TokenKind_literal_int) {
        return ast_new_int(atoi(string_to_cstr(&t->raw)));
    } else if (t->kind == TokenKind_literal_str) {
        e = ast_new(AstNodeKind_str_expr);
        e->node_idx = register_str_literal(p, string_to_cstr(&t->raw));
        return e;
    } else if (t->kind == TokenKind_paren_l) {
        e = parse_expr(p);
        expect(p, TokenKind_paren_r);
        return e;
    } else if (t->kind == TokenKind_ident) {
        String* name = &t->raw;

        if (peek_token(p)->kind == TokenKind_paren_l) {
            e = ast_new(AstNodeKind_func_call);
            int func_idx = find_func(p, name);
            if (func_idx == -1) {
                buf = calloc(1024, sizeof(char));
                sprintf(buf, "undefined function: %.*s", name->len, name->data);
                fatal_error(buf);
            }
            e->name.data = name->data;
            e->name.len = name->len;
            e->ty = p->funcs[func_idx].ty;
            return e;
        }

        int lvar_idx = find_lvar(p, name);
        if (lvar_idx == -1) {
            int gvar_idx = find_gvar(p, name);
            if (gvar_idx == -1) {
                int enum_member_idx = find_enum_member(p, name);
                if (enum_member_idx == -1) {
                    buf = calloc(1024, sizeof(char));
                    sprintf(buf, "undefined variable: %.*s", name->len, name->data);
                    fatal_error(buf);
                }
                int enum_idx = enum_member_idx / 1000;
                int n = enum_member_idx % 1000;
                e = ast_new_int(p->enums[enum_idx].node_members->node_items[n].node_int_value);
                e->ty = type_new(TypeKind_enum);
                e->ty->def = p->enums + enum_idx;
                return e;
            }
            e = ast_new(AstNodeKind_gvar);
            e->name.data = name->data;
            e->name.len = name->len;
            e->ty = p->gvars[gvar_idx].ty;
            return e;
        }

        e = ast_new(AstNodeKind_lvar);
        e->name.data = name->data;
        e->name.len = name->len;
        e->node_idx = lvar_idx;
        e->ty = p->lvars[lvar_idx].ty;
        return e;
    } else {
        buf = calloc(1024, sizeof(char));
        sprintf(buf, "expected primary expression, but got '%s'", token_stringify(t));
        fatal_error(buf);
    }
}

AstNode* parse_arg_list(Parser* p) {
    AstNode* list = ast_new_list(6);
    while (peek_token(p)->kind != TokenKind_paren_r) {
        AstNode* arg = parse_expr(p);
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

AstNode* parse_postfix_expr(Parser* p) {
    AstNode* ret = parse_primary_expr(p);
    String* name;
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

int is_type_token(Parser* p, Token* token) {
    if (token->kind == TokenKind_keyword_int || token->kind == TokenKind_keyword_long ||
        token->kind == TokenKind_keyword_char || token->kind == TokenKind_keyword_void ||
        token->kind == TokenKind_keyword_enum || token->kind == TokenKind_keyword_struct ||
        token->kind == TokenKind_keyword_const) {
        return 1;
    }
    if (token->kind != TokenKind_ident) {
        return 0;
    }
    return find_typedef(p, &token->raw) != -1;
}

Type* parse_type(Parser* p) {
    Token* t = next_token(p);
    char* buf;
    String* name;
    if (t->kind == TokenKind_keyword_const) {
        t = next_token(p);
    }
    if (!is_type_token(p, t)) {
        buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_type: expected type, but got '%s'", token_stringify(t));
        fatal_error(buf);
    }
    Type* ty;
    if (t->kind == TokenKind_ident) {
        int typedef_idx = find_typedef(p, &t->raw);
        if (typedef_idx == -1) {
            buf = calloc(1024, sizeof(char));
            sprintf(buf, "parse_type: unknown typedef, %.*s", t->raw.len, t->raw.data);
            fatal_error(buf);
        }
        ty = p->typedefs[typedef_idx].ty;
    } else {
        ty = type_new(TypeKind_unknown);
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
                sprintf(buf, "parse_type: unknown enum, %.*s", name->len, name->data);
                fatal_error(buf);
            }
            ty->def = p->enums + enum_idx;
        } else if (t->kind == TokenKind_keyword_struct) {
            ty->kind = TypeKind_struct;
            name = parse_ident(p);
            int struct_idx = find_struct(p, name);
            if (struct_idx == -1) {
                buf = calloc(1024, sizeof(char));
                sprintf(buf, "parse_type: unknown struct, %.*s", name->len, name->data);
                fatal_error(buf);
            }
            ty->def = p->structs + struct_idx;
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
    AstNode* operand;
    TokenKind op = peek_token(p)->kind;
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
        Type* ty = parse_type(p);
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
    AstNode* rhs;
    while (1) {
        TokenKind op = peek_token(p)->kind;
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
                if (rhs->ty->kind == TypeKind_ptr) {
                    // (a - b) / sizeof(a)
                    lhs = ast_new_binary_expr(TokenKind_slash, ast_new_binary_expr(op, lhs, rhs),
                                              ast_new_int(type_sizeof(lhs->ty->to)));
                } else {
                    // a - b*sizeof(a)
                    lhs = ast_new_binary_expr(
                        op, lhs, ast_new_binary_expr(TokenKind_star, rhs, ast_new_int(type_sizeof(lhs->ty->to))));
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

AstNode* parse_relational_expr(Parser* p) {
    AstNode* lhs = parse_additive_expr(p);
    AstNode* rhs;
    while (1) {
        TokenKind op = peek_token(p)->kind;
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

AstNode* parse_logical_and_expr(Parser* p) {
    AstNode* lhs = parse_equality_expr(p);
    while (1) {
        TokenKind op = peek_token(p)->kind;
        if (op == TokenKind_andand) {
            next_token(p);
            AstNode* rhs = parse_equality_expr(p);
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
    AstNode* rhs;
    while (1) {
        TokenKind op = peek_token(p)->kind;
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

AstNode* parse_expr(Parser* p) {
    return parse_assignment_expr(p);
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

AstNode* parse_for_stmt(Parser* p) {
    expect(p, TokenKind_keyword_for);
    expect(p, TokenKind_paren_l);
    AstNode* init = NULL;
    AstNode* cond = NULL;
    AstNode* update = NULL;
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
    AstNode* body = parse_stmt(p);

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

AstNode* parse_var_decl(Parser* p) {
    Type* ty = parse_type(p);
    if (!type_is_unsized(ty)) {
        fatal_error("parse_var_decl: invalid type for variable");
    }
    String* name = parse_ident(p);

    AstNode* init = NULL;
    if (peek_token(p)->kind == TokenKind_assign) {
        next_token(p);
        init = parse_expr(p);
    }
    expect(p, TokenKind_semicolon);

    if (find_lvar(p, name) != -1 || find_gvar(p, name) != -1) {
        char* buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_var_decl: %.*s redeclared", name->len, name->data);
        fatal_error(buf);
    }
    p->lvars[p->n_lvars].name.data = name->data;
    p->lvars[p->n_lvars].name.len = name->len;
    p->lvars[p->n_lvars].ty = ty;
    ++p->n_lvars;

    AstNode* ret;
    if (init) {
        AstNode* lhs = ast_new(AstNodeKind_lvar);
        lhs->name.data = name->data;
        lhs->name.len = name->len;
        lhs->node_idx = p->n_lvars - 1;
        lhs->ty = ty;
        AstNode* assign = ast_new_assign_expr(TokenKind_assign, lhs, init);
        ret = ast_new(AstNodeKind_expr_stmt);
        ret->node_expr = assign;
    } else {
        ret = ast_new(AstNodeKind_lvar_decl);
    }
    return ret;
}

AstNode* parse_expr_stmt(Parser* p) {
    AstNode* e = parse_expr(p);
    expect(p, TokenKind_semicolon);
    AstNode* stmt = ast_new(AstNodeKind_expr_stmt);
    stmt->node_expr = e;
    return stmt;
}

AstNode* parse_block_stmt(Parser* p) {
    AstNode* list = ast_new_list(1024);
    expect(p, TokenKind_brace_l);
    while (peek_token(p)->kind != TokenKind_brace_r) {
        AstNode* stmt = parse_stmt(p);
        ast_append(list, stmt);
    }
    expect(p, TokenKind_brace_r);
    return list;
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
    } else if (is_type_token(p, t)) {
        return parse_var_decl(p);
    } else {
        return parse_expr_stmt(p);
    }
}

void enter_func(Parser* p) {
    p->lvars = calloc(LVAR_MAX, sizeof(LocalVar));
    p->n_lvars = 0;
}

void register_params(Parser* p, AstNode* params) {
    int i;
    for (i = 0; i < params->node_len; ++i) {
        AstNode* param = params->node_items + i;
        p->lvars[p->n_lvars].name.data = param->name.data;
        p->lvars[p->n_lvars].name.len = param->name.len;
        p->lvars[p->n_lvars].ty = param->ty;
        ++p->n_lvars;
    }
}

void register_func(Parser* p, const String* name, Type* ty) {
    p->funcs[p->n_funcs].name.data = name->data;
    p->funcs[p->n_funcs].name.len = name->len;
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
        param->name.data = name->data;
        param->name.len = name->len;
    }
    return param;
}

AstNode* parse_param_list(Parser* p) {
    int has_void = 0;
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

AstNode* parse_func_decl_or_def(Parser* p) {
    Type* ty = parse_type(p);
    String* name = parse_ident(p);
    register_func(p, name, ty);
    expect(p, TokenKind_paren_l);
    AstNode* params = parse_param_list(p);
    expect(p, TokenKind_paren_r);
    if (peek_token(p)->kind == TokenKind_semicolon) {
        next_token(p);
        return ast_new(AstNodeKind_func_decl);
    }
    enter_func(p);
    register_params(p, params);
    AstNode* body = parse_block_stmt(p);
    AstNode* func = ast_new(AstNodeKind_func_def);
    func->ty = ty;
    func->name.data = name->data;
    func->name.len = name->len;
    func->node_params = params;
    func->node_body = body;
    return func;
}

AstNode* parse_struct_member(Parser* p) {
    Type* ty = parse_type(p);
    String* name = parse_ident(p);
    expect(p, TokenKind_semicolon);
    AstNode* member = ast_new(AstNodeKind_struct_member);
    member->name.data = name->data;
    member->name.len = name->len;
    member->ty = ty;
    return member;
}

AstNode* parse_struct_members(Parser* p) {
    AstNode* list = ast_new_list(16);
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
        p->structs[struct_idx].name.data = name->data;
        p->structs[struct_idx].name.len = name->len;
        ++p->n_structs;
    }
    if (peek_token(p)->kind == TokenKind_semicolon) {
        next_token(p);
        return ast_new(AstNodeKind_struct_decl);
    }
    if (p->structs[struct_idx].node_members) {
        char* buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_struct_decl_or_def: struct %.*s redefined", name->len, name->data);
        fatal_error(buf);
    }
    expect(p, TokenKind_brace_l);
    AstNode* members = parse_struct_members(p);
    expect(p, TokenKind_brace_r);
    expect(p, TokenKind_semicolon);
    p->structs[struct_idx].node_members = members;
    return p->structs + struct_idx;
}

AstNode* parse_enum_member(Parser* p) {
    String* name = parse_ident(p);
    AstNode* member = ast_new(AstNodeKind_enum_member);
    member->name.data = name->data;
    member->name.len = name->len;
    return member;
}

AstNode* parse_enum_members(Parser* p) {
    int next_value = 0;
    AstNode* list = ast_new_list(256);
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
        p->enums[enum_idx].name.data = name->data;
        p->enums[enum_idx].name.len = name->len;
        ++p->n_enums;
    } else {
        char* buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_enum_def: enum %.*s redefined", name->len, name->data);
        fatal_error(buf);
    }
    expect(p, TokenKind_brace_l);
    AstNode* members = parse_enum_members(p);
    expect(p, TokenKind_brace_r);
    expect(p, TokenKind_semicolon);
    p->enums[enum_idx].node_members = members;
    return p->enums + enum_idx;
}

AstNode* parse_typedef_decl(Parser* p) {
    expect(p, TokenKind_keyword_typeof);
    Type* ty = parse_type(p);
    String* name = parse_ident(p);
    expect(p, TokenKind_semicolon);
    AstNode* decl = ast_new(AstNodeKind_typedef_decl);
    decl->name.data = name->data;
    decl->name.len = name->len;
    decl->ty = ty;
    p->typedefs[p->n_typedefs].name.data = name->data;
    p->typedefs[p->n_typedefs].name.len = name->len;
    p->typedefs[p->n_typedefs].ty = ty;
    ++p->n_typedefs;
    return decl;
}

AstNode* parse_extern_var_decl(Parser* p) {
    expect(p, TokenKind_keyword_extern);
    Type* ty = parse_type(p);
    if (!type_is_unsized(ty)) {
        fatal_error("parse_extern_var_decl: invalid type for variable");
    }
    String* name = parse_ident(p);
    expect(p, TokenKind_semicolon);

    if (find_lvar(p, name) != -1 || find_gvar(p, name) != -1) {
        char* buf = calloc(1024, sizeof(char));
        sprintf(buf, "parse_extern_var_decl: %.*s redeclared", name->len, name->data);
        fatal_error(buf);
    }
    p->gvars[p->n_gvars].name.data = name->data;
    p->gvars[p->n_gvars].name.len = name->len;
    p->gvars[p->n_gvars].ty = ty;
    ++p->n_gvars;

    return ast_new(AstNodeKind_gvar_decl);
}

AstNode* parse_toplevel(Parser* p) {
    TokenKind tk = peek_token(p)->kind;
    if (tk == TokenKind_keyword_struct) {
        return parse_struct_decl_or_def(p);
    } else if (tk == TokenKind_keyword_enum) {
        return parse_enum_def(p);
    } else if (tk == TokenKind_keyword_typeof) {
        return parse_typedef_decl(p);
    } else if (tk == TokenKind_keyword_extern) {
        return parse_extern_var_decl(p);
    } else {
        return parse_func_decl_or_def(p);
    }
}

Program* parse(Token* tokens) {
    Parser* p = parser_new(tokens);
    AstNode* funcs = ast_new_list(1024);
    while (eof(p)) {
        AstNode* n = parse_toplevel(p);
        if (n->kind == AstNodeKind_func_def) {
            ast_append(funcs, n);
        }
    }
    Program* prog = calloc(1, sizeof(Program));
    prog->funcs = funcs;
    prog->str_literals = p->str_literals;
    return prog;
}

void analyze(Program* prog) {
}

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

int main(int argc, char** argv) {
    if (argc == 1) {
        fatal_error("usage: ducc <FILE>");
    }
    FILE* in;
    if (strcmp(argv[1], "-") == 0) {
        in = stdin;
    } else {
        in = fopen(argv[1], "rb");
    }
    char* source = read_all(in);
    PpToken* pp_tokens = preprocess(source);
    Token* tokens = tokenize(pp_tokens);
    Program* prog = parse(tokens);
    analyze(prog);
    codegen(prog);
    return 0;
}
