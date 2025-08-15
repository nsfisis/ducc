enum TokenKind {
    TokenKind_eof,

    // Only preprocessing phase.
    TokenKind_hash,
    TokenKind_hashhash,
    TokenKind_whitespace,
    TokenKind_other,
    TokenKind_character_constant,

    TokenKind_and,
    TokenKind_andand,
    TokenKind_arrow,
    TokenKind_assign,
    TokenKind_assign_add,
    TokenKind_assign_and,
    TokenKind_assign_div,
    TokenKind_assign_lshift,
    TokenKind_assign_mod,
    TokenKind_assign_mul,
    TokenKind_assign_or,
    TokenKind_assign_rshift,
    TokenKind_assign_sub,
    TokenKind_assign_xor,
    TokenKind_brace_l,
    TokenKind_brace_r,
    TokenKind_bracket_l,
    TokenKind_bracket_r,
    TokenKind_colon,
    TokenKind_comma,
    TokenKind_dot,
    TokenKind_ellipsis,
    TokenKind_eq,
    TokenKind_ge,
    TokenKind_gt,
    TokenKind_ident,
    TokenKind_keyword__Bool,
    TokenKind_keyword__Complex,
    TokenKind_keyword__Imaginary,
    TokenKind_keyword_auto,
    TokenKind_keyword_break,
    TokenKind_keyword_case,
    TokenKind_keyword_char,
    TokenKind_keyword_const,
    TokenKind_keyword_continue,
    TokenKind_keyword_default,
    TokenKind_keyword_do,
    TokenKind_keyword_double,
    TokenKind_keyword_else,
    TokenKind_keyword_enum,
    TokenKind_keyword_extern,
    TokenKind_keyword_float,
    TokenKind_keyword_for,
    TokenKind_keyword_goto,
    TokenKind_keyword_if,
    TokenKind_keyword_inline,
    TokenKind_keyword_int,
    TokenKind_keyword_long,
    TokenKind_keyword_register,
    TokenKind_keyword_restrict,
    TokenKind_keyword_return,
    TokenKind_keyword_short,
    TokenKind_keyword_signed,
    TokenKind_keyword_sizeof,
    TokenKind_keyword_static,
    TokenKind_keyword_struct,
    TokenKind_keyword_switch,
    TokenKind_keyword_typedef,
    TokenKind_keyword_union,
    TokenKind_keyword_unsigned,
    TokenKind_keyword_void,
    TokenKind_keyword_volatile,
    TokenKind_keyword_while,
    TokenKind_le,
    TokenKind_literal_int,
    TokenKind_literal_str,
    TokenKind_lshift,
    TokenKind_lt,
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
    TokenKind_question,
    TokenKind_rshift,
    TokenKind_semicolon,
    TokenKind_slash,
    TokenKind_star,
    TokenKind_tilde,
    TokenKind_xor,

    // va_start() is currently implemented as a special form due to the current limitation of #define macro.
    TokenKind_va_start,
};
typedef enum TokenKind TokenKind;

const char* token_kind_stringify(TokenKind k) {
    if (k == TokenKind_eof)
        return "<eof>";
    else if (k == TokenKind_hash)
        return "#";
    else if (k == TokenKind_hashhash)
        return "##";
    else if (k == TokenKind_whitespace)
        return "<whitespace>";
    else if (k == TokenKind_other)
        return "<other>";
    else if (k == TokenKind_character_constant)
        return "<character-constant>";
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
    else if (k == TokenKind_assign_and)
        return "&=";
    else if (k == TokenKind_assign_div)
        return "/=";
    else if (k == TokenKind_assign_lshift)
        return "<<=";
    else if (k == TokenKind_assign_mod)
        return "%=";
    else if (k == TokenKind_assign_mul)
        return "*=";
    else if (k == TokenKind_assign_or)
        return "|=";
    else if (k == TokenKind_assign_rshift)
        return ">>=";
    else if (k == TokenKind_assign_sub)
        return "-=";
    else if (k == TokenKind_assign_xor)
        return "^=";
    else if (k == TokenKind_brace_l)
        return "{";
    else if (k == TokenKind_brace_r)
        return "}";
    else if (k == TokenKind_bracket_l)
        return "[";
    else if (k == TokenKind_bracket_r)
        return "]";
    else if (k == TokenKind_colon)
        return ":";
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
    else if (k == TokenKind_keyword__Bool)
        return "_Bool";
    else if (k == TokenKind_keyword__Complex)
        return "_Complex";
    else if (k == TokenKind_keyword__Imaginary)
        return "_Imaginary";
    else if (k == TokenKind_keyword_auto)
        return "auto";
    else if (k == TokenKind_keyword_break)
        return "break";
    else if (k == TokenKind_keyword_case)
        return "case";
    else if (k == TokenKind_keyword_char)
        return "char";
    else if (k == TokenKind_keyword_const)
        return "const";
    else if (k == TokenKind_keyword_continue)
        return "continue";
    else if (k == TokenKind_keyword_default)
        return "default";
    else if (k == TokenKind_keyword_do)
        return "do";
    else if (k == TokenKind_keyword_double)
        return "double";
    else if (k == TokenKind_keyword_else)
        return "else";
    else if (k == TokenKind_keyword_enum)
        return "enum";
    else if (k == TokenKind_keyword_extern)
        return "extern";
    else if (k == TokenKind_keyword_float)
        return "float";
    else if (k == TokenKind_keyword_for)
        return "for";
    else if (k == TokenKind_keyword_goto)
        return "goto";
    else if (k == TokenKind_keyword_if)
        return "if";
    else if (k == TokenKind_keyword_inline)
        return "inline";
    else if (k == TokenKind_keyword_int)
        return "int";
    else if (k == TokenKind_keyword_long)
        return "long";
    else if (k == TokenKind_keyword_register)
        return "register";
    else if (k == TokenKind_keyword_restrict)
        return "restrict";
    else if (k == TokenKind_keyword_return)
        return "return";
    else if (k == TokenKind_keyword_short)
        return "short";
    else if (k == TokenKind_keyword_signed)
        return "signed";
    else if (k == TokenKind_keyword_sizeof)
        return "sizeof";
    else if (k == TokenKind_keyword_static)
        return "static";
    else if (k == TokenKind_keyword_struct)
        return "struct";
    else if (k == TokenKind_keyword_switch)
        return "switch";
    else if (k == TokenKind_keyword_typedef)
        return "typedef";
    else if (k == TokenKind_keyword_union)
        return "union";
    else if (k == TokenKind_keyword_unsigned)
        return "unsigned";
    else if (k == TokenKind_keyword_void)
        return "void";
    else if (k == TokenKind_keyword_volatile)
        return "volatile";
    else if (k == TokenKind_keyword_while)
        return "while";
    else if (k == TokenKind_le)
        return "le";
    else if (k == TokenKind_literal_int)
        return "<integer>";
    else if (k == TokenKind_literal_str)
        return "<string>";
    else if (k == TokenKind_lshift)
        return "<<";
    else if (k == TokenKind_lt)
        return "lt";
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
    else if (k == TokenKind_question)
        return "?";
    else if (k == TokenKind_rshift)
        return ">>";
    else if (k == TokenKind_semicolon)
        return ";";
    else if (k == TokenKind_slash)
        return "/";
    else if (k == TokenKind_star)
        return "*";
    else if (k == TokenKind_tilde)
        return "~";
    else if (k == TokenKind_xor)
        return "^";
    else if (k == TokenKind_va_start)
        return "va_start";
    else
        unreachable();
}

struct SourceLocation {
    const char* filename;
    int line;
};
typedef struct SourceLocation SourceLocation;

struct Token {
    TokenKind kind;
    String raw;
    SourceLocation loc;
};
typedef struct Token Token;

const char* token_stringify(Token* t) {
    TokenKind k = t->kind;
    if (k == TokenKind_other || k == TokenKind_character_constant || k == TokenKind_whitespace ||
        k == TokenKind_ident || k == TokenKind_literal_int || k == TokenKind_literal_str) {
        const char* kind_str = token_kind_stringify(k);
        char* buf = calloc(t->raw.len + strlen(kind_str) + 3 + 1, sizeof(char));
        sprintf(buf, "%.*s (%s)", t->raw.len, t->raw.data, kind_str);
        return buf;
    } else {
        return token_kind_stringify(k);
    }
}

struct TokenArray {
    size_t len;
    size_t capacity;
    Token* data;
};
typedef struct TokenArray TokenArray;

void tokens_init(TokenArray* tokens, size_t capacity) {
    tokens->len = 0;
    tokens->capacity = capacity;
    tokens->data = calloc(tokens->capacity, sizeof(Token));
}

void tokens_reserve(TokenArray* tokens, size_t size) {
    if (size <= tokens->capacity)
        return;
    tokens->capacity *= 2;
    tokens->data = realloc(tokens->data, tokens->capacity * sizeof(Token));
    memset(tokens->data + tokens->len, 0, (tokens->capacity - tokens->len) * sizeof(Token));
}

Token* tokens_push_new(TokenArray* tokens) {
    tokens_reserve(tokens, tokens->len + 1);
    return &tokens->data[tokens->len++];
}

Token* tokens_pop(TokenArray* tokens) {
    if (tokens->len != 0)
        tokens->len--;
}

enum MacroKind {
    MacroKind_obj,
    MacroKind_func,
    MacroKind_builtin_file,
    MacroKind_builtin_line,
};
typedef enum MacroKind MacroKind;

const char* macro_kind_stringify(MacroKind kind) {
    if (kind == MacroKind_obj)
        return "object-like";
    else if (kind == MacroKind_func)
        return "function-like";
    else if (kind == MacroKind_builtin_file)
        return "__FILE__";
    else if (kind == MacroKind_builtin_line)
        return "__LINE__";
    else
        unreachable();
}

struct Macro {
    MacroKind kind;
    String name;
    TokenArray replacements;
};
typedef struct Macro Macro;

struct MacroArray {
    size_t len;
    size_t capacity;
    Macro* data;
};
typedef struct MacroArray MacroArray;

MacroArray* macros_new() {
    MacroArray* macros = calloc(1, sizeof(MacroArray));
    macros->len = 0;
    macros->capacity = 8;
    macros->data = calloc(macros->capacity, sizeof(Macro));
    return macros;
}

void macros_reserve(MacroArray* macros, size_t size) {
    if (size <= macros->capacity)
        return;
    macros->capacity *= 2;
    macros->data = realloc(macros->data, macros->capacity * sizeof(Macro));
    memset(macros->data + macros->len, 0, (macros->capacity - macros->len) * sizeof(Macro));
}

Macro* macros_push_new(MacroArray* macros) {
    macros_reserve(macros, macros->len + 1);
    return &macros->data[macros->len++];
}

void macros_dump(MacroArray* macros) {
    fprintf(stderr, "MacroArray {\n");
    fprintf(stderr, "  len = %zu\n", macros->len);
    fprintf(stderr, "  data = [\n");
    for (int i = 0; i < macros->len; ++i) {
        Macro* m = &macros->data[i];
        fprintf(stderr, "    Macro {\n");
        fprintf(stderr, "      kind = %s\n", macro_kind_stringify(m->kind));
        fprintf(stderr, "      name = %.*s\n", m->name.len, m->name.data);
        fprintf(stderr, "      replacements = TODO\n");
        fprintf(stderr, "    }\n");
    }
    fprintf(stderr, "  ]\n");
    fprintf(stderr, "}\n");
}

void add_predefined_macros(MacroArray* macros) {
    Macro* m;

    m = macros_push_new(macros);
    m->kind = MacroKind_obj;
    m->name.len = strlen("__ducc__");
    m->name.data = "__ducc__";
    tokens_init(&m->replacements, 1);
    Token* tok = tokens_push_new(&m->replacements);
    tok->kind = TokenKind_literal_int;
    tok->raw.len = strlen("1");
    tok->raw.data = "1";

    m = macros_push_new(macros);
    m->kind = MacroKind_builtin_file;
    m->name.len = strlen("__FILE__");
    m->name.data = "__FILE__";

    m = macros_push_new(macros);
    m->kind = MacroKind_builtin_line;
    m->name.len = strlen("__LINE__");
    m->name.data = "__LINE__";
}

struct PpLexer {
    const char* filename;
    int line;
    char* src;
    int pos;
    TokenArray* pp_tokens;
};
typedef struct PpLexer PpLexer;

PpLexer* pplexer_new(InFile* src) {
    PpLexer* ppl = calloc(1, sizeof(PpLexer));

    ppl->filename = src->filename;
    ppl->line = 1;
    ppl->src = src->buf;
    ppl->pp_tokens = calloc(1, sizeof(TokenArray));
    tokens_init(ppl->pp_tokens, 1024 * 16);

    return ppl;
}

void pplexer_tokenize_all(PpLexer* ppl) {
    while (ppl->src[ppl->pos]) {
        Token* tok = tokens_push_new(ppl->pp_tokens);
        tok->loc.filename = ppl->filename;
        tok->loc.line = ppl->line;
        char c = ppl->src[ppl->pos];
        ++ppl->pos;
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
        } else if (c == ':') {
            tok->kind = TokenKind_colon;
        } else if (c == ';') {
            tok->kind = TokenKind_semicolon;
        } else if (c == '^') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_assign_xor;
            } else {
                tok->kind = TokenKind_xor;
            }
        } else if (c == '?') {
            tok->kind = TokenKind_question;
        } else if (c == '~') {
            tok->kind = TokenKind_tilde;
        } else if (c == '+') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_assign_add;
            } else if (ppl->src[ppl->pos] == '+') {
                ++ppl->pos;
                tok->kind = TokenKind_plusplus;
            } else {
                tok->kind = TokenKind_plus;
            }
        } else if (c == '|') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_assign_or;
            } else if (ppl->src[ppl->pos] == '|') {
                ++ppl->pos;
                tok->kind = TokenKind_oror;
            } else {
                tok->kind = TokenKind_or;
            }
        } else if (c == '&') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_assign_and;
            } else if (ppl->src[ppl->pos] == '&') {
                ++ppl->pos;
                tok->kind = TokenKind_andand;
            } else {
                tok->kind = TokenKind_and;
            }
        } else if (c == '-') {
            if (ppl->src[ppl->pos] == '>') {
                ++ppl->pos;
                tok->kind = TokenKind_arrow;
            } else if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_assign_sub;
            } else if (ppl->src[ppl->pos] == '-') {
                ++ppl->pos;
                tok->kind = TokenKind_minusminus;
            } else {
                tok->kind = TokenKind_minus;
            }
        } else if (c == '*') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_assign_mul;
            } else {
                tok->kind = TokenKind_star;
            }
        } else if (c == '/') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_assign_div;
            } else if (ppl->src[ppl->pos] == '/') {
                int start = ppl->pos - 1;
                ++ppl->pos;
                while (ppl->src[ppl->pos] && ppl->src[ppl->pos] != '\n' && ppl->src[ppl->pos] != '\r') {
                    ++ppl->pos;
                }
                tok->kind = TokenKind_whitespace;
                tok->raw.len = ppl->pos - start;
                tok->raw.data = ppl->src + ppl->pos - tok->raw.len;
            } else if (ppl->src[ppl->pos] == '*') {
                int start = ppl->pos - 1;
                ++ppl->pos;
                while (ppl->src[ppl->pos]) {
                    if (ppl->src[ppl->pos] == '*' && ppl->src[ppl->pos + 1] == '/') {
                        ppl->pos += 2;
                        break;
                    }
                    if (ppl->src[ppl->pos] == '\n') {
                        ++ppl->line;
                    }
                    ++ppl->pos;
                }
                tok->kind = TokenKind_whitespace;
                tok->raw.len = ppl->pos - start;
                tok->raw.data = ppl->src + ppl->pos - tok->raw.len;
            } else {
                tok->kind = TokenKind_slash;
            }
        } else if (c == '%') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_assign_mod;
            } else {
                tok->kind = TokenKind_percent;
            }
        } else if (c == '.') {
            if (ppl->src[ppl->pos] == '.') {
                ++ppl->pos;
                if (ppl->src[ppl->pos] == '.') {
                    ++ppl->pos;
                    tok->kind = TokenKind_ellipsis;
                } else {
                    tok->kind = TokenKind_other;
                    tok->raw.len = 2;
                    tok->raw.data = ppl->src + ppl->pos - tok->raw.len;
                }
            } else {
                tok->kind = TokenKind_dot;
                tok->raw.len = 1;
                tok->raw.data = ppl->src + ppl->pos - tok->raw.len;
            }
        } else if (c == '!') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_ne;
            } else {
                tok->kind = TokenKind_not;
            }
        } else if (c == '=') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_eq;
            } else {
                tok->kind = TokenKind_assign;
            }
        } else if (c == '<') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_le;
            } else if (ppl->src[ppl->pos] == '<') {
                ++ppl->pos;
                if (ppl->src[ppl->pos] == '=') {
                    ++ppl->pos;
                    tok->kind = TokenKind_assign_lshift;
                } else {
                    tok->kind = TokenKind_lshift;
                }
            } else {
                tok->kind = TokenKind_lt;
            }
        } else if (c == '>') {
            if (ppl->src[ppl->pos] == '=') {
                ++ppl->pos;
                tok->kind = TokenKind_ge;
            } else if (ppl->src[ppl->pos] == '>') {
                ++ppl->pos;
                if (ppl->src[ppl->pos] == '=') {
                    ++ppl->pos;
                    tok->kind = TokenKind_assign_rshift;
                } else {
                    tok->kind = TokenKind_rshift;
                }
            } else {
                tok->kind = TokenKind_gt;
            }
        } else if (c == '#') {
            if (ppl->src[ppl->pos] == '#') {
                ++ppl->pos;
                tok->kind = TokenKind_hashhash;
            } else {
                tok->kind = TokenKind_hash;
            }
        } else if (c == '\'') {
            int start = ppl->pos - 1;
            if (ppl->src[ppl->pos] == '\\') {
                ++ppl->pos;
            }
            ppl->pos += 2;
            tok->kind = TokenKind_character_constant;
            tok->raw.data = ppl->src + start;
            tok->raw.len = ppl->pos - start;
        } else if (c == '"') {
            int start = ppl->pos - 1;
            while (1) {
                char ch = ppl->src[ppl->pos];
                if (ch == '\\') {
                    ++ppl->pos;
                } else if (ch == '"') {
                    break;
                }
                ++ppl->pos;
            }
            ++ppl->pos;
            tok->kind = TokenKind_literal_str;
            tok->raw.data = ppl->src + start;
            tok->raw.len = ppl->pos - start;
        } else if (isdigit(c)) {
            --ppl->pos;
            int start = ppl->pos;
            while (isdigit(ppl->src[ppl->pos])) {
                ++ppl->pos;
            }
            tok->kind = TokenKind_literal_int;
            tok->raw.data = ppl->src + start;
            tok->raw.len = ppl->pos - start;
        } else if (isalpha(c) || c == '_') {
            --ppl->pos;
            int start = ppl->pos;
            while (isalnum(ppl->src[ppl->pos]) || ppl->src[ppl->pos] == '_') {
                ++ppl->pos;
            }
            tok->raw.data = ppl->src + start;
            tok->raw.len = ppl->pos - start;
            if (string_equals_cstr(&tok->raw, "auto")) {
                tok->kind = TokenKind_keyword_auto;
            } else if (string_equals_cstr(&tok->raw, "break")) {
                tok->kind = TokenKind_keyword_break;
            } else if (string_equals_cstr(&tok->raw, "case")) {
                tok->kind = TokenKind_keyword_case;
            } else if (string_equals_cstr(&tok->raw, "char")) {
                tok->kind = TokenKind_keyword_char;
            } else if (string_equals_cstr(&tok->raw, "const")) {
                tok->kind = TokenKind_keyword_const;
            } else if (string_equals_cstr(&tok->raw, "continue")) {
                tok->kind = TokenKind_keyword_continue;
            } else if (string_equals_cstr(&tok->raw, "default")) {
                tok->kind = TokenKind_keyword_default;
            } else if (string_equals_cstr(&tok->raw, "do")) {
                tok->kind = TokenKind_keyword_do;
            } else if (string_equals_cstr(&tok->raw, "double")) {
                tok->kind = TokenKind_keyword_double;
            } else if (string_equals_cstr(&tok->raw, "else")) {
                tok->kind = TokenKind_keyword_else;
            } else if (string_equals_cstr(&tok->raw, "enum")) {
                tok->kind = TokenKind_keyword_enum;
            } else if (string_equals_cstr(&tok->raw, "extern")) {
                tok->kind = TokenKind_keyword_extern;
            } else if (string_equals_cstr(&tok->raw, "float")) {
                tok->kind = TokenKind_keyword_float;
            } else if (string_equals_cstr(&tok->raw, "for")) {
                tok->kind = TokenKind_keyword_for;
            } else if (string_equals_cstr(&tok->raw, "goto")) {
                tok->kind = TokenKind_keyword_goto;
            } else if (string_equals_cstr(&tok->raw, "if")) {
                tok->kind = TokenKind_keyword_if;
            } else if (string_equals_cstr(&tok->raw, "inline")) {
                tok->kind = TokenKind_keyword_inline;
            } else if (string_equals_cstr(&tok->raw, "int")) {
                tok->kind = TokenKind_keyword_int;
            } else if (string_equals_cstr(&tok->raw, "long")) {
                tok->kind = TokenKind_keyword_long;
            } else if (string_equals_cstr(&tok->raw, "register")) {
                tok->kind = TokenKind_keyword_register;
            } else if (string_equals_cstr(&tok->raw, "restrict")) {
                tok->kind = TokenKind_keyword_restrict;
            } else if (string_equals_cstr(&tok->raw, "return")) {
                tok->kind = TokenKind_keyword_return;
            } else if (string_equals_cstr(&tok->raw, "short")) {
                tok->kind = TokenKind_keyword_short;
            } else if (string_equals_cstr(&tok->raw, "signed")) {
                tok->kind = TokenKind_keyword_signed;
            } else if (string_equals_cstr(&tok->raw, "sizeof")) {
                tok->kind = TokenKind_keyword_sizeof;
            } else if (string_equals_cstr(&tok->raw, "static")) {
                tok->kind = TokenKind_keyword_static;
            } else if (string_equals_cstr(&tok->raw, "struct")) {
                tok->kind = TokenKind_keyword_struct;
            } else if (string_equals_cstr(&tok->raw, "switch")) {
                tok->kind = TokenKind_keyword_switch;
            } else if (string_equals_cstr(&tok->raw, "typedef")) {
                tok->kind = TokenKind_keyword_typedef;
            } else if (string_equals_cstr(&tok->raw, "union")) {
                tok->kind = TokenKind_keyword_union;
            } else if (string_equals_cstr(&tok->raw, "unsigned")) {
                tok->kind = TokenKind_keyword_unsigned;
            } else if (string_equals_cstr(&tok->raw, "void")) {
                tok->kind = TokenKind_keyword_void;
            } else if (string_equals_cstr(&tok->raw, "volatile")) {
                tok->kind = TokenKind_keyword_volatile;
            } else if (string_equals_cstr(&tok->raw, "while")) {
                tok->kind = TokenKind_keyword_while;
            } else if (string_equals_cstr(&tok->raw, "_Bool")) {
                tok->kind = TokenKind_keyword__Bool;
            } else if (string_equals_cstr(&tok->raw, "_Complex")) {
                tok->kind = TokenKind_keyword__Complex;
            } else if (string_equals_cstr(&tok->raw, "_Imaginary")) {
                tok->kind = TokenKind_keyword__Imaginary;
            } else if (string_equals_cstr(&tok->raw, "va_start")) {
                tok->kind = TokenKind_va_start;
            } else {
                tok->kind = TokenKind_ident;
            }
        } else if (isspace(c)) {
            --ppl->pos;
            int start = ppl->pos;
            while (isspace((c = ppl->src[ppl->pos]))) {
                if (c == '\n' || c == '\r') {
                    ++ppl->line;
                }
                ++ppl->pos;
            }
            tok->kind = TokenKind_whitespace;
            tok->raw.data = ppl->src + start;
            tok->raw.len = ppl->pos - start;
        } else {
            tok->kind = TokenKind_other;
            tok->raw.len = 1;
            tok->raw.data = ppl->src + ppl->pos - tok->raw.len;
        }
    }
    Token* eof_tok = tokens_push_new(ppl->pp_tokens);
    eof_tok->loc.filename = ppl->filename;
    eof_tok->loc.line = ppl->line;
    eof_tok->kind = TokenKind_eof;
}

TokenArray* pp_tokenize(InFile* src) {
    PpLexer* ppl = pplexer_new(src);
    pplexer_tokenize_all(ppl);
    return ppl->pp_tokens;
}

struct Preprocessor {
    TokenArray* pp_tokens;
    int pos;
    MacroArray* macros;
    int include_depth;
    BOOL skip_pp_tokens;
    String* include_paths;
    int n_include_paths;
};
typedef struct Preprocessor Preprocessor;

TokenArray* do_preprocess(InFile* src, int depth, MacroArray* macros);

Preprocessor* preprocessor_new(TokenArray* pp_tokens, int include_depth, MacroArray* macros) {
    if (include_depth >= 32) {
        fatal_error("include depth limit exceeded");
    }

    Preprocessor* pp = calloc(1, sizeof(Preprocessor));
    pp->pp_tokens = pp_tokens;
    pp->macros = macros;
    pp->include_depth = include_depth;
    pp->include_paths = calloc(16, sizeof(String));

    return pp;
}

Token* pp_token_at(Preprocessor* pp, int i) {
    return &pp->pp_tokens->data[i];
}

Token* peek_pp_token(Preprocessor* pp) {
    return pp_token_at(pp, pp->pos);
}

Token* next_pp_token(Preprocessor* pp) {
    return pp_token_at(pp, pp->pos++);
}

BOOL pp_eof(Preprocessor* pp) {
    return peek_pp_token(pp)->kind == TokenKind_eof;
}

int find_macro(Preprocessor* pp, String* name) {
    for (int i = 0; i < pp->macros->len; ++i) {
        if (string_equals(&pp->macros->data[i].name, name)) {
            return i;
        }
    }
    return -1;
}

void undef_macro(Preprocessor* pp, int idx) {
    pp->macros->data[idx].name.len = 0;
    // TODO: Can predefined macro like __FILE__ be undefined?
}

void add_include_path(Preprocessor* pp, char* include_path) {
    pp->include_paths[pp->n_include_paths].data = include_path;
    pp->include_paths[pp->n_include_paths].len = strlen(include_path);
    ++pp->n_include_paths;
}

BOOL skip_pp_tokens(Preprocessor* pp) {
    // TODO: support nested #if
    return pp->skip_pp_tokens;
}

void skip_whitespaces(Preprocessor* pp) {
    while (!pp_eof(pp) && peek_pp_token(pp)->kind == TokenKind_whitespace) {
        next_pp_token(pp);
    }
}

BOOL string_contains_newline(String* s) {
    for (int i = 0; i < s->len; ++i) {
        if (s->data[i] == '\n') {
            return TRUE;
        }
    }
    return FALSE;
}

void seek_to_next_newline(Preprocessor* pp) {
    while (!pp_eof(pp)) {
        Token* tok = peek_pp_token(pp);
        if (tok->kind == TokenKind_whitespace && string_contains_newline(&tok->raw)) {
            break;
        }
        next_pp_token(pp);
    }
}

void make_token_whitespace(Token* tok) {
    tok->kind = TokenKind_whitespace;
    tok->raw.len = 0;
    tok->raw.data = NULL;
}

void remove_directive_tokens(Preprocessor* pp, int start, int end) {
    for (int i = start; i < end; ++i) {
        make_token_whitespace(pp_token_at(pp, i));
    }
}

void process_endif_directive(Preprocessor* pp, int hash_pos) {
    next_pp_token(pp);
    pp->skip_pp_tokens = FALSE;
    remove_directive_tokens(pp, hash_pos, pp->pos);
}

void process_else_directive(Preprocessor* pp, int hash_pos) {
    next_pp_token(pp);
    pp->skip_pp_tokens = !pp->skip_pp_tokens;
    remove_directive_tokens(pp, hash_pos, pp->pos);
}

void process_elif_directive(Preprocessor* pp, int hash_pos) {
    unimplemented();
}

void process_if_directive(Preprocessor* pp, int hash_pos) {
    unimplemented();
}

void process_ifdef_directive(Preprocessor* pp, int hash_pos) {
    next_pp_token(pp);
    skip_whitespaces(pp);
    Token* macro_name = peek_pp_token(pp);
    if (macro_name->kind == TokenKind_ident) {
        next_pp_token(pp);
        pp->skip_pp_tokens = find_macro(pp, &macro_name->raw) == -1;
    }
    remove_directive_tokens(pp, hash_pos, pp->pos);
}

void process_ifndef_directive(Preprocessor* pp, int hash_pos) {
    next_pp_token(pp);
    skip_whitespaces(pp);
    Token* macro_name = peek_pp_token(pp);
    if (macro_name->kind == TokenKind_ident) {
        next_pp_token(pp);
        pp->skip_pp_tokens = find_macro(pp, &macro_name->raw) != -1;
    }
    remove_directive_tokens(pp, hash_pos, pp->pos);
}

String* read_include_header_name(Preprocessor* pp) {
    Token* tok = next_pp_token(pp);
    if (tok->kind == TokenKind_literal_str) {
        return &tok->raw;
    } else if (tok->kind == TokenKind_lt) {
        char* include_name_start = peek_pp_token(pp)->raw.data;
        int include_name_len = 0;
        while (!pp_eof(pp)) {
            if (peek_pp_token(pp)->kind == TokenKind_gt) {
                break;
            }
            include_name_len += peek_pp_token(pp)->raw.len;
            next_pp_token(pp);
        }
        if (pp_eof(pp)) {
            fatal_error("invalid #include: <> not balanced");
        }
        next_pp_token(pp);
        String* include_name = calloc(1, sizeof(String));
        include_name->data = include_name_start;
        include_name->len = include_name_len;
        return include_name;
    } else {
        unreachable();
    }
}

const char* resolve_include_name(Preprocessor* pp, String* include_name) {
    if (include_name->data[0] == '"') {
        char* buf = calloc(include_name->len - 2 + 1, sizeof(char));
        sprintf(buf, "%.*s", include_name->len - 2, include_name->data + 1);
        return buf;
    } else {
        for (int i = 0; i < pp->n_include_paths; ++i) {
            char* buf = calloc(include_name->len + 1 + pp->include_paths[i].len, sizeof(char));
            sprintf(buf, "%s/%.*s", pp->include_paths[i].data, include_name->len, include_name->data);
            if (access(buf, F_OK | R_OK) == 0) {
                return buf;
            }
        }
        return NULL;
    }
}

int replace_pp_tokens(Preprocessor* pp, int dest_start, int dest_end, TokenArray* source_tokens) {
    int n_tokens_to_remove = dest_end - dest_start;
    int n_tokens_after_dest = pp->pp_tokens->len - dest_end;
    int shift_amount;

    if (n_tokens_to_remove < source_tokens->len) {
        // Move existing tokens backward to make room.
        shift_amount = source_tokens->len - n_tokens_to_remove;
        tokens_reserve(pp->pp_tokens, pp->pp_tokens->len + shift_amount);
        memmove(pp_token_at(pp, dest_end + shift_amount), pp_token_at(pp, dest_end),
                n_tokens_after_dest * sizeof(Token));
        pp->pp_tokens->len += shift_amount;
    } else if (source_tokens->len < n_tokens_to_remove) {
        // Move existing tokens forward to reduce room.
        shift_amount = n_tokens_to_remove - source_tokens->len;
        memmove(pp_token_at(pp, dest_start + source_tokens->len), pp_token_at(pp, dest_end),
                n_tokens_after_dest * sizeof(Token));
        pp->pp_tokens->len -= shift_amount;
        memset(pp_token_at(pp, pp->pp_tokens->len), 0, shift_amount * sizeof(Token));
    }

    memcpy(pp_token_at(pp, dest_start), source_tokens->data, source_tokens->len * sizeof(Token));

    return dest_start + source_tokens->len;
}

void expand_include_directive(Preprocessor* pp, int hash_pos, const char* include_name_buf) {
    InFile* include_source = read_all(include_name_buf);
    if (!include_source) {
        fatal_error("cannot open include file: %s", include_name_buf);
    }

    TokenArray* include_pp_tokens = do_preprocess(include_source, pp->include_depth + 1, pp->macros);
    tokens_pop(include_pp_tokens); // pop EOF token
    pp->pos = replace_pp_tokens(pp, hash_pos, pp->pos, include_pp_tokens);
}

void process_include_directive(Preprocessor* pp, int hash_pos) {
    next_pp_token(pp);
    skip_whitespaces(pp);
    String* include_name = read_include_header_name(pp);
    const char* include_name_buf = resolve_include_name(pp, include_name);
    if (include_name_buf == NULL) {
        fatal_error("cannot resolve include file name: %.*s", include_name->len, include_name->data);
    }
    expand_include_directive(pp, hash_pos, include_name_buf);
}

void process_define_directive(Preprocessor* pp, int hash_pos) {
    next_pp_token(pp);
    skip_whitespaces(pp);
    Token* macro_name = next_pp_token(pp);

    if (macro_name->kind != TokenKind_ident) {
        fatal_error("%s:%s: invalid #define syntax", macro_name->loc.filename, macro_name->loc.line);
    }

    if (peek_pp_token(pp)->kind == TokenKind_paren_l) {
        next_pp_token(pp);
        if (peek_pp_token(pp)->kind != TokenKind_paren_r) {
            unimplemented();
        }
        next_pp_token(pp);
        int replacements_start_pos = pp->pos;
        seek_to_next_newline(pp);
        if (pp_eof(pp)) {
            fatal_error("%s:%s: invalid #define syntax");
        }
        Macro* macro = macros_push_new(pp->macros);
        macro->kind = MacroKind_func;
        macro->name = macro_name->raw;
        int n_replacements = pp->pos - replacements_start_pos;
        tokens_init(&macro->replacements, n_replacements);
        for (int i = 0; i < n_replacements; ++i) {
            *tokens_push_new(&macro->replacements) = *pp_token_at(pp, replacements_start_pos + i);
        }
    } else {
        int replacements_start_pos = pp->pos;
        seek_to_next_newline(pp);
        if (pp_eof(pp)) {
            fatal_error("%s:%s: invalid #define syntax");
        }
        Macro* macro = macros_push_new(pp->macros);
        macro->kind = MacroKind_obj;
        macro->name = macro_name->raw;
        int n_replacements = pp->pos - replacements_start_pos;
        tokens_init(&macro->replacements, n_replacements);
        for (int i = 0; i < n_replacements; ++i) {
            *tokens_push_new(&macro->replacements) = *pp_token_at(pp, replacements_start_pos + i);
        }
    }
    remove_directive_tokens(pp, hash_pos, pp->pos);
}

void process_undef_directive(Preprocessor* pp, int hash_pos) {
    next_pp_token(pp);
    skip_whitespaces(pp);
    Token* macro_name = peek_pp_token(pp);
    if (macro_name->kind == TokenKind_ident) {
        next_pp_token(pp);
        int macro_idx = find_macro(pp, &macro_name->raw);
        if (macro_idx != -1) {
            undef_macro(pp, macro_idx);
        }
    }
    remove_directive_tokens(pp, hash_pos, pp->pos);
}

void process_line_directive(Preprocessor* pp, int hash_pos) {
    unimplemented();
}

void process_error_directive(Preprocessor* pp, int hash_pos) {
    unimplemented();
}

void process_pragma_directive(Preprocessor* pp, int hash_pos) {
    unimplemented();
}

BOOL expand_macro(Preprocessor* pp) {
    int macro_name_pos = pp->pos;
    Token* macro_name = next_pp_token(pp);
    int macro_idx = find_macro(pp, &macro_name->raw);
    if (macro_idx == -1) {
        return FALSE;
    }

    SourceLocation original_loc = macro_name->loc;
    Macro* macro = &pp->macros->data[macro_idx];
    if (macro->kind == MacroKind_func) {
        // also consume '(' and ')'
        replace_pp_tokens(pp, macro_name_pos, macro_name_pos + 3, &macro->replacements);
        // Inherit a source location from the original macro token.
        for (int i = 0; i < macro->replacements.len; ++i) {
            pp_token_at(pp, macro_name_pos + i)->loc = original_loc;
        }
    } else if (macro->kind == MacroKind_obj) {
        replace_pp_tokens(pp, macro_name_pos, macro_name_pos + 1, &macro->replacements);
        // Inherit a source location from the original macro token.
        for (int i = 0; i < macro->replacements.len; ++i) {
            pp_token_at(pp, macro_name_pos + i)->loc = original_loc;
        }
    } else if (macro->kind == MacroKind_builtin_file) {
        TokenArray tokens;
        tokens_init(&tokens, 1);
        Token* file_tok = tokens_push_new(&tokens);
        file_tok->kind = TokenKind_literal_str;
        file_tok->raw.len = strlen(macro_name->loc.filename) + 2;
        file_tok->raw.data = calloc(file_tok->raw.len, sizeof(char));
        sprintf(file_tok->raw.data, "\"%s\"", macro_name->loc.filename);
        replace_pp_tokens(pp, macro_name_pos, macro_name_pos + 1, &tokens);
    } else if (macro->kind == MacroKind_builtin_line) {
        TokenArray tokens;
        tokens_init(&tokens, 1);
        Token* line_tok = tokens_push_new(&tokens);
        line_tok->kind = TokenKind_literal_int;
        line_tok->raw.data = calloc(10, sizeof(char));
        sprintf(line_tok->raw.data, "%d", macro_name->loc.line);
        line_tok->raw.len = strlen(line_tok->raw.data);
        replace_pp_tokens(pp, macro_name_pos, macro_name_pos + 1, &tokens);
    } else {
        unreachable();
    }
    return TRUE;
}

BOOL is_pp_hash(Token* t) {
    // TODO: '#' must be at the beginning of the line.
    return t->kind == TokenKind_hash;
}

void process_pp_directive(Preprocessor* pp) {
    int first_token_pos = pp->pos;
    Token* first_token = peek_pp_token(pp);
    if (is_pp_hash(first_token)) {
        next_pp_token(pp);
        // TODO: don't skip newline after '#'.
        skip_whitespaces(pp);
        Token* next_tok = peek_pp_token(pp);
        if (next_tok->kind == TokenKind_ident && string_equals_cstr(&next_tok->raw, "endif")) {
            process_endif_directive(pp, first_token_pos);
        } else if (next_tok->kind == TokenKind_keyword_else) {
            process_else_directive(pp, first_token_pos);
        } else if (next_tok->kind == TokenKind_ident && string_equals_cstr(&next_tok->raw, "elif")) {
            process_elif_directive(pp, first_token_pos);
        } else if (skip_pp_tokens(pp)) {
            make_token_whitespace(pp_token_at(pp, first_token_pos));
            make_token_whitespace(next_pp_token(pp));
        } else if (next_tok->kind == TokenKind_keyword_if) {
            process_if_directive(pp, first_token_pos);
        } else if (next_tok->kind == TokenKind_ident && string_equals_cstr(&next_tok->raw, "ifdef")) {
            process_ifdef_directive(pp, first_token_pos);
        } else if (next_tok->kind == TokenKind_ident && string_equals_cstr(&next_tok->raw, "ifndef")) {
            process_ifndef_directive(pp, first_token_pos);
        } else if (next_tok->kind == TokenKind_ident && string_equals_cstr(&next_tok->raw, "include")) {
            process_include_directive(pp, first_token_pos);
        } else if (next_tok->kind == TokenKind_ident && string_equals_cstr(&next_tok->raw, "define")) {
            process_define_directive(pp, first_token_pos);
        } else if (next_tok->kind == TokenKind_ident && string_equals_cstr(&next_tok->raw, "undef")) {
            process_undef_directive(pp, first_token_pos);
        } else if (next_tok->kind == TokenKind_ident && string_equals_cstr(&next_tok->raw, "line")) {
            process_line_directive(pp, first_token_pos);
        } else if (next_tok->kind == TokenKind_ident && string_equals_cstr(&next_tok->raw, "error")) {
            process_error_directive(pp, first_token_pos);
        } else if (next_tok->kind == TokenKind_ident && string_equals_cstr(&next_tok->raw, "pragma")) {
            process_pragma_directive(pp, first_token_pos);
        } else {
            fatal_error("%s:%d: unknown preprocessor directive (%s)", next_tok->loc.filename, next_tok->loc.line,
                        token_stringify(next_tok));
        }
    } else if (skip_pp_tokens(pp)) {
        make_token_whitespace(next_pp_token(pp));
    } else if (first_token->kind == TokenKind_ident) {
        BOOL expanded = expand_macro(pp);
        if (expanded) {
            // A macro may expand to another macro. Re-scan the expanded tokens.
            // TODO: if the macro is defined recursively, it causes infinite loop.
        } else {
            next_pp_token(pp);
        }
    } else {
        next_pp_token(pp);
    }
}

void process_pp_directives(Preprocessor* pp) {
    while (!pp_eof(pp)) {
        process_pp_directive(pp);
    }
}

void pp_dump(Token* t, BOOL include_whitespace) {
    for (; t->kind != TokenKind_eof; ++t) {
        if (t->kind == TokenKind_whitespace && !include_whitespace) {
            continue;
        }
        fprintf(stderr, "%s\n", token_stringify(t));
    }
}

char* get_ducc_include_path() {
    const char* self_dir = get_self_dir();
    char* buf = calloc(strlen(self_dir) + strlen("/include") + 1, sizeof(char));
    sprintf(buf, "%s/include", self_dir);
    return buf;
}

TokenArray* do_preprocess(InFile* src, int depth, MacroArray* macros) {
    TokenArray* pp_tokens = pp_tokenize(src);
    Preprocessor* pp = preprocessor_new(pp_tokens, depth, macros);
    add_include_path(pp, get_ducc_include_path());
    add_include_path(pp, "/usr/include/x86_64-linux-gnu");
    add_include_path(pp, "/usr/include");
    process_pp_directives(pp);
    return pp->pp_tokens;
}

TokenArray* preprocess(InFile* src) {
    MacroArray* macros = macros_new();
    add_predefined_macros(macros);
    return do_preprocess(src, 0, macros);
}
