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
    TokenKind_assign_div,
    TokenKind_assign_mod,
    TokenKind_assign_mul,
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
    TokenKind_keyword__Bool,
    TokenKind_keyword__Complex,
    TokenKind_keyword__Imaginary,

    TokenKind_le,
    TokenKind_lshift,
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
    TokenKind_rshift,
    TokenKind_semicolon,
    TokenKind_slash,
    TokenKind_star,

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
    else if (k == TokenKind_assign_div)
        return "/=";
    else if (k == TokenKind_assign_mod)
        return "%=";
    else if (k == TokenKind_assign_mul)
        return "*=";
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
    else if (k == TokenKind_keyword__Bool)
        return "_Bool";
    else if (k == TokenKind_keyword__Complex)
        return "_Complex";
    else if (k == TokenKind_keyword__Imaginary)
        return "_Imaginary";
    else if (k == TokenKind_le)
        return "le";
    else if (k == TokenKind_lshift)
        return "<<";
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
    else if (k == TokenKind_rshift)
        return ">>";
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

struct Preprocessor {
    const char* filename;
    int line;
    char* src;
    int pos;
    TokenArray* pp_tokens;
    MacroArray* macros;
    int include_depth;
    BOOL skip_pp_tokens;
    String* include_paths;
    int n_include_paths;
};
typedef struct Preprocessor Preprocessor;

TokenArray* do_preprocess(InFile* src, int depth, MacroArray* macros);

Preprocessor* preprocessor_new(InFile* src, int include_depth, MacroArray* macros) {
    if (include_depth >= 32) {
        fatal_error("include depth limit exceeded");
    }

    Preprocessor* pp = calloc(1, sizeof(Preprocessor));
    pp->filename = src->filename;
    pp->line = 1;
    pp->src = src->buf;
    pp->pp_tokens = calloc(1, sizeof(TokenArray));
    tokens_init(pp->pp_tokens, 1024 * 16);
    pp->macros = macros;
    pp->include_depth = include_depth;
    pp->include_paths = calloc(16, sizeof(String));

    return pp;
}

Token* pp_token_at(Preprocessor* pp, int i) {
    return &pp->pp_tokens->data[i];
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

void pp_tokenize_all(Preprocessor* pp) {
    while (pp->src[pp->pos]) {
        Token* tok = tokens_push_new(pp->pp_tokens);
        tok->loc.filename = pp->filename;
        tok->loc.line = pp->line;
        char c = pp->src[pp->pos];
        ++pp->pos;
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
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = TokenKind_assign_add;
            } else if (pp->src[pp->pos] == '+') {
                ++pp->pos;
                tok->kind = TokenKind_plusplus;
            } else {
                tok->kind = TokenKind_plus;
            }
        } else if (c == '|') {
            if (pp->src[pp->pos] == '|') {
                ++pp->pos;
                tok->kind = TokenKind_oror;
            } else {
                tok->kind = TokenKind_or;
            }
        } else if (c == '&') {
            if (pp->src[pp->pos] == '&') {
                ++pp->pos;
                tok->kind = TokenKind_andand;
            } else {
                tok->kind = TokenKind_and;
            }
        } else if (c == '-') {
            if (pp->src[pp->pos] == '>') {
                ++pp->pos;
                tok->kind = TokenKind_arrow;
            } else if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = TokenKind_assign_sub;
            } else if (pp->src[pp->pos] == '-') {
                ++pp->pos;
                tok->kind = TokenKind_minusminus;
            } else {
                tok->kind = TokenKind_minus;
            }
        } else if (c == '*') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = TokenKind_assign_mul;
            } else {
                tok->kind = TokenKind_star;
            }
        } else if (c == '/') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = TokenKind_assign_div;
            } else if (pp->src[pp->pos] == '/') {
                int start = pp->pos - 1;
                ++pp->pos;
                while (pp->src[pp->pos] && pp->src[pp->pos] != '\n' && pp->src[pp->pos] != '\r') {
                    ++pp->pos;
                }
                tok->kind = TokenKind_whitespace;
                tok->raw.len = pp->pos - start;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else if (pp->src[pp->pos] == '*') {
                int start = pp->pos - 1;
                ++pp->pos;
                while (pp->src[pp->pos]) {
                    if (pp->src[pp->pos] == '*' && pp->src[pp->pos + 1] == '/') {
                        pp->pos += 2;
                        break;
                    }
                    if (pp->src[pp->pos] == '\n') {
                        ++pp->line;
                    }
                    ++pp->pos;
                }
                tok->kind = TokenKind_whitespace;
                tok->raw.len = pp->pos - start;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else {
                tok->kind = TokenKind_slash;
            }
        } else if (c == '%') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = TokenKind_assign_mod;
            } else {
                tok->kind = TokenKind_percent;
            }
        } else if (c == '.') {
            if (pp->src[pp->pos] == '.') {
                ++pp->pos;
                if (pp->src[pp->pos] == '.') {
                    ++pp->pos;
                    tok->kind = TokenKind_ellipsis;
                } else {
                    tok->kind = TokenKind_other;
                    tok->raw.len = 2;
                    tok->raw.data = pp->src + pp->pos - tok->raw.len;
                }
            } else {
                tok->kind = TokenKind_dot;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
        } else if (c == '!') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = TokenKind_ne;
            } else {
                tok->kind = TokenKind_not;
            }
        } else if (c == '=') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = TokenKind_eq;
            } else {
                tok->kind = TokenKind_assign;
            }
        } else if (c == '<') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = TokenKind_le;
            } else if (pp->src[pp->pos] == '<') {
                ++pp->pos;
                tok->kind = TokenKind_lshift;
            } else {
                tok->kind = TokenKind_lt;
            }
        } else if (c == '>') {
            if (pp->src[pp->pos] == '=') {
                ++pp->pos;
                tok->kind = TokenKind_ge;
            } else if (pp->src[pp->pos] == '>') {
                ++pp->pos;
                tok->kind = TokenKind_rshift;
            } else {
                tok->kind = TokenKind_gt;
            }
        } else if (c == '#') {
            if (pp->src[pp->pos] == '#') {
                ++pp->pos;
                tok->kind = TokenKind_hashhash;
            } else {
                tok->kind = TokenKind_hash;
            }
        } else if (c == '\'') {
            int start = pp->pos - 1;
            if (pp->src[pp->pos] == '\\') {
                ++pp->pos;
            }
            pp->pos += 2;
            tok->kind = TokenKind_character_constant;
            tok->raw.data = pp->src + start;
            tok->raw.len = pp->pos - start;
        } else if (c == '"') {
            int start = pp->pos - 1;
            while (1) {
                char ch = pp->src[pp->pos];
                if (ch == '\\') {
                    ++pp->pos;
                } else if (ch == '"') {
                    break;
                }
                ++pp->pos;
            }
            ++pp->pos;
            tok->kind = TokenKind_literal_str;
            tok->raw.data = pp->src + start;
            tok->raw.len = pp->pos - start;
        } else if (isdigit(c)) {
            --pp->pos;
            int start = pp->pos;
            while (isdigit(pp->src[pp->pos])) {
                ++pp->pos;
            }
            tok->kind = TokenKind_literal_int;
            tok->raw.data = pp->src + start;
            tok->raw.len = pp->pos - start;
        } else if (isalpha(c) || c == '_') {
            --pp->pos;
            int start = pp->pos;
            while (isalnum(pp->src[pp->pos]) || pp->src[pp->pos] == '_') {
                ++pp->pos;
            }
            tok->raw.data = pp->src + start;
            tok->raw.len = pp->pos - start;
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
            if (c == '\n' || c == '\r') {
                ++pp->line;
            }
            tok->kind = TokenKind_whitespace;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else {
            tok->kind = TokenKind_other;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        }
    }
    Token* eof_tok = tokens_push_new(pp->pp_tokens);
    eof_tok->loc.filename = pp->filename;
    eof_tok->loc.line = pp->line;
    eof_tok->kind = TokenKind_eof;
}

int skip_whitespace(Preprocessor* pp, int pos) {
    for (; pos < pp->pp_tokens->len; ++pos) {
        if (pp_token_at(pp, pos)->kind != TokenKind_whitespace) {
            break;
        }
    }
    return pos;
}

BOOL string_contains_newline(String* s) {
    for (int i = 0; i < s->len; ++i) {
        if (s->data[i] == '\n') {
            return TRUE;
        }
    }
    return FALSE;
}

int find_next_newline(Preprocessor* pp, int pos) {
    for (; pos < pp->pp_tokens->len; ++pos) {
        if (pp_token_at(pp, pos)->kind == TokenKind_whitespace && string_contains_newline(&pp_token_at(pp, pos)->raw)) {
            return pos;
        }
    }
    return -1;
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

int process_endif_directive(Preprocessor* pp, int tok, int tok2) {
    ++tok2;
    pp->skip_pp_tokens = FALSE;
    remove_directive_tokens(pp, tok, tok2);
    return tok2;
}

int process_else_directive(Preprocessor* pp, int tok, int tok2) {
    ++tok2;
    pp->skip_pp_tokens = !pp->skip_pp_tokens;
    remove_directive_tokens(pp, tok, tok2);
    return tok2;
}

int process_elif_directive(Preprocessor* pp, int tok, int tok2) {
    unimplemented();
}

int process_if_directive(Preprocessor* pp, int tok, int tok2) {
    unimplemented();
}

int process_ifdef_directive(Preprocessor* pp, int tok, int tok2) {
    ++tok2;
    tok2 = skip_whitespace(pp, tok2);
    if (pp_token_at(pp, tok2)->kind == TokenKind_ident) {
        Token* name = pp_token_at(pp, tok2);
        ++tok2;
        pp->skip_pp_tokens = find_macro(pp, &name->raw) == -1;
    }
    remove_directive_tokens(pp, tok, tok2);
    return tok2;
}

int process_ifndef_directive(Preprocessor* pp, int tok, int tok2) {
    ++tok2;
    tok2 = skip_whitespace(pp, tok2);
    if (pp_token_at(pp, tok2)->kind == TokenKind_ident) {
        Token* name = pp_token_at(pp, tok2);
        ++tok2;
        pp->skip_pp_tokens = find_macro(pp, &name->raw) != -1;
    }
    remove_directive_tokens(pp, tok, tok2);
    return tok2;
}

int read_include_header_name(Preprocessor* pp, int tok2, String* include_name) {
    if (pp_token_at(pp, tok2)->kind == TokenKind_literal_str) {
        *include_name = pp_token_at(pp, tok2)->raw;
        ++tok2;
        return tok2;
    } else if (pp_token_at(pp, tok2)->kind == TokenKind_lt) {
        ++tok2;
        char* include_name_start = pp_token_at(pp, tok2)->raw.data;
        int include_name_len = 0;
        while (pp_token_at(pp, tok2)->kind != TokenKind_eof) {
            if (pp_token_at(pp, tok2)->kind == TokenKind_gt) {
                break;
            }
            include_name_len += pp_token_at(pp, tok2)->raw.len;
            ++tok2;
        }
        if (pp_token_at(pp, tok2)->kind == TokenKind_eof) {
            fatal_error("invalid #include: <> not balanced");
        }
        ++tok2;
        include_name->data = include_name_start;
        include_name->len = include_name_len;
        return tok2;
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
        memmove(pp->pp_tokens->data + dest_end + shift_amount, pp->pp_tokens->data + dest_end,
                n_tokens_after_dest * sizeof(Token));
        pp->pp_tokens->len += shift_amount;
    } else if (source_tokens->len < n_tokens_to_remove) {
        // Move existing tokens forward to reduce room.
        shift_amount = n_tokens_to_remove - source_tokens->len;
        memmove(pp->pp_tokens->data + dest_start + source_tokens->len, pp->pp_tokens->data + dest_end,
                n_tokens_after_dest * sizeof(Token));
        pp->pp_tokens->len -= shift_amount;
        memset(pp->pp_tokens->data + pp->pp_tokens->len, 0, shift_amount * sizeof(Token));
    }

    memcpy(pp->pp_tokens->data + dest_start, source_tokens->data, source_tokens->len * sizeof(Token));

    return dest_start + source_tokens->len;
}

int expand_include_directive(Preprocessor* pp, int tok, int tok2, const char* include_name_buf) {
    InFile* include_source = read_all(include_name_buf);
    if (!include_source) {
        fatal_error("cannot open include file: %s", include_name_buf);
    }

    TokenArray* include_pp_tokens = do_preprocess(include_source, pp->include_depth + 1, pp->macros);
    tokens_pop(include_pp_tokens); // pop EOF token
    return replace_pp_tokens(pp, tok, tok2 + 1, include_pp_tokens);
}

int process_include_directive(Preprocessor* pp, int tok, int tok2) {
    ++tok2;
    tok2 = skip_whitespace(pp, tok2);
    String* include_name = calloc(1, sizeof(String));
    tok2 = read_include_header_name(pp, tok2, include_name);
    const char* include_name_buf = resolve_include_name(pp, include_name);
    if (include_name_buf == NULL) {
        fatal_error("cannot resolve include file name: %.*s", include_name->len, include_name->data);
    }
    return expand_include_directive(pp, tok, tok2, include_name_buf);
}

int process_define_directive(Preprocessor* pp, int tok, int tok2) {
    int tok3 = -1;
    ++tok2;
    tok2 = skip_whitespace(pp, tok2);
    if (pp_token_at(pp, tok2)->kind != TokenKind_ident) {
        fatal_error("%s:%s: invalid #define syntax", pp_token_at(pp, tok2)->loc.filename,
                    pp_token_at(pp, tok2)->loc.line);
    }

    Token* macro_name = pp_token_at(pp, tok2);
    ++tok2;
    if (pp_token_at(pp, tok2)->kind == TokenKind_paren_l) {
        ++tok2;
        if (pp_token_at(pp, tok2)->kind == TokenKind_paren_r) {
            ++tok2;
        } else {
            fatal_error("%s:%d: invalid function-like macro syntax (#define %.*s)", macro_name->loc.filename,
                        macro_name->loc.line, macro_name->raw.len, macro_name->raw.data);
        }
        tok3 = find_next_newline(pp, tok2);
        if (tok3 == -1) {
            fatal_error("%s:%s: invalid #define syntax", pp_token_at(pp, tok3)->loc.filename,
                        pp_token_at(pp, tok3)->loc.line);
        }
        Macro* macro = macros_push_new(pp->macros);
        macro->kind = MacroKind_func;
        macro->name = macro_name->raw;
        int n_replacements = tok3 - tok2;
        tokens_init(&macro->replacements, n_replacements);
        for (int i = 0; i < n_replacements; ++i) {
            *tokens_push_new(&macro->replacements) = *pp_token_at(pp, tok2 + i);
        }
    } else {
        tok3 = find_next_newline(pp, tok2);
        if (tok3 == -1) {
            fatal_error("%s:%s: invalid #define syntax", pp_token_at(pp, tok3)->loc.filename,
                        pp_token_at(pp, tok3)->loc.line);
        }
        Macro* macro = macros_push_new(pp->macros);
        macro->kind = MacroKind_obj;
        macro->name = macro_name->raw;
        int n_replacements = tok3 - tok2;
        tokens_init(&macro->replacements, n_replacements);
        for (int i = 0; i < n_replacements; ++i) {
            *tokens_push_new(&macro->replacements) = *pp_token_at(pp, tok2 + i);
        }
    }
    remove_directive_tokens(pp, tok, tok3);
    return tok3;
}

int process_undef_directive(Preprocessor* pp, int tok, int tok2) {
    tok2 = skip_whitespace(pp, tok2 + 1);
    if (pp_token_at(pp, tok2)->kind == TokenKind_ident) {
        Token* macro_name = pp_token_at(pp, tok2);
        ++tok2;
        int macro_idx = find_macro(pp, &macro_name->raw);
        if (macro_idx != -1) {
            undef_macro(pp, macro_idx);
        }
    }
    remove_directive_tokens(pp, tok, tok2);
    return tok2;
}

int process_line_directive(Preprocessor* pp, int tok, int tok2) {
    unimplemented();
}

int process_error_directive(Preprocessor* pp, int tok, int tok2) {
    unimplemented();
}

int process_pragma_directive(Preprocessor* pp, int tok, int tok2) {
    unimplemented();
}

BOOL expand_macro(Preprocessor* pp, int tok) {
    int macro_idx = find_macro(pp, &pp_token_at(pp, tok)->raw);
    if (macro_idx == -1) {
        return FALSE;
    }

    SourceLocation original_loc = pp_token_at(pp, tok)->loc;
    Macro* macro = pp->macros->data + macro_idx;
    if (macro->kind == MacroKind_func) {
        // also consume '(' and ')'
        replace_pp_tokens(pp, tok, tok + 3, &macro->replacements);
        // Inherit a source location from the original macro token.
        for (int i = 0; i < macro->replacements.len; ++i) {
            pp_token_at(pp, tok + i)->loc = original_loc;
        }
    } else if (macro->kind == MacroKind_obj) {
        replace_pp_tokens(pp, tok, tok + 1, &macro->replacements);
        // Inherit a source location from the original macro token.
        for (int i = 0; i < macro->replacements.len; ++i) {
            pp_token_at(pp, tok + i)->loc = original_loc;
        }
    } else if (macro->kind == MacroKind_builtin_file) {
        TokenArray tokens;
        tokens_init(&tokens, 1);
        Token* file_tok = tokens_push_new(&tokens);
        file_tok->kind = TokenKind_literal_str;
        file_tok->raw.len = strlen(pp_token_at(pp, tok)->loc.filename) + 2;
        file_tok->raw.data = calloc(file_tok->raw.len, sizeof(char));
        sprintf(file_tok->raw.data, "\"%s\"", pp_token_at(pp, tok)->loc.filename);
        replace_pp_tokens(pp, tok, tok + 1, &tokens);
    } else if (macro->kind == MacroKind_builtin_line) {
        TokenArray tokens;
        tokens_init(&tokens, 1);
        Token* line_tok = tokens_push_new(&tokens);
        line_tok->kind = TokenKind_literal_int;
        line_tok->raw.data = calloc(10, sizeof(char));
        sprintf(line_tok->raw.data, "%d", pp_token_at(pp, tok)->loc.line);
        line_tok->raw.len = strlen(line_tok->raw.data);
        replace_pp_tokens(pp, tok, tok + 1, &tokens);
    } else {
        unreachable();
    }
    return TRUE;
}

BOOL is_pp_hash(Token* t) {
    // TODO: '#' must be at the beginning of the line.
    return t->kind == TokenKind_hash;
}

void process_pp_directives(Preprocessor* pp) {
    int tok = 0;

    while (pp_token_at(pp, tok)->kind != TokenKind_eof) {
        if (is_pp_hash(pp_token_at(pp, tok))) {
            // TODO: don't skip newline after '#'.
            int tok2 = skip_whitespace(pp, tok + 1);
            if (pp_token_at(pp, tok2)->kind == TokenKind_ident &&
                string_equals_cstr(&pp_token_at(pp, tok2)->raw, "endif")) {
                tok = process_endif_directive(pp, tok, tok2);
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_keyword_else) {
                tok = process_else_directive(pp, tok, tok2);
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_ident &&
                       string_equals_cstr(&pp_token_at(pp, tok2)->raw, "elif")) {
                tok = process_elif_directive(pp, tok, tok2);
            } else if (skip_pp_tokens(pp)) {
                make_token_whitespace(pp_token_at(pp, tok));
                ++tok;
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_keyword_if) {
                tok = process_if_directive(pp, tok, tok2);
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_ident &&
                       string_equals_cstr(&pp_token_at(pp, tok2)->raw, "ifdef")) {
                tok = process_ifdef_directive(pp, tok, tok2);
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_ident &&
                       string_equals_cstr(&pp_token_at(pp, tok2)->raw, "ifndef")) {
                tok = process_ifndef_directive(pp, tok, tok2);
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_ident &&
                       string_equals_cstr(&pp_token_at(pp, tok2)->raw, "include")) {
                tok = process_include_directive(pp, tok, tok2);
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_ident &&
                       string_equals_cstr(&pp_token_at(pp, tok2)->raw, "define")) {
                tok = process_define_directive(pp, tok, tok2);
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_ident &&
                       string_equals_cstr(&pp_token_at(pp, tok2)->raw, "undef")) {
                tok = process_undef_directive(pp, tok, tok2);
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_ident &&
                       string_equals_cstr(&pp_token_at(pp, tok2)->raw, "line")) {
                tok = process_line_directive(pp, tok, tok2);
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_ident &&
                       string_equals_cstr(&pp_token_at(pp, tok2)->raw, "error")) {
                tok = process_error_directive(pp, tok, tok2);
            } else if (pp_token_at(pp, tok2)->kind == TokenKind_ident &&
                       string_equals_cstr(&pp_token_at(pp, tok2)->raw, "pragma")) {
                tok = process_pragma_directive(pp, tok, tok2);
            } else {
                fatal_error("%s:%d: unknown preprocessor directive (%s)", pp_token_at(pp, tok2)->loc.filename,
                            pp_token_at(pp, tok2)->loc.line, token_stringify(pp_token_at(pp, tok2)));
            }
        } else if (skip_pp_tokens(pp)) {
            make_token_whitespace(pp_token_at(pp, tok));
            ++tok;
        } else if (pp_token_at(pp, tok)->kind == TokenKind_ident) {
            BOOL expanded = expand_macro(pp, tok);
            if (expanded) {
                // A macro may expand to another macro. Re-scan the expanded tokens.
                // TODO: if the macro is defined recursively, it causes infinite loop.
            } else {
                ++tok;
            }
        } else {
            ++tok;
        }
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
    Preprocessor* pp = preprocessor_new(src, depth, macros);
    add_include_path(pp, get_ducc_include_path());
    add_include_path(pp, "/usr/include/x86_64-linux-gnu");
    add_include_path(pp, "/usr/include");
    pp_tokenize_all(pp);
    process_pp_directives(pp);
    return pp->pp_tokens;
}

TokenArray* preprocess(InFile* src) {
    MacroArray* macros = macros_new();
    add_predefined_macros(macros);
    return do_preprocess(src, 0, macros);
}
