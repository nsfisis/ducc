#include "preprocess.h"
#include "common.h"
#include "parse.h"
#include "sys.h"

const char* token_kind_stringify(TokenKind k) {
    if (k == TokenKind_eof)
        return "<eof>";
    else if (k == TokenKind_hash)
        return "#";
    else if (k == TokenKind_hashhash)
        return "##";
    else if (k == TokenKind_whitespace)
        return "<whitespace>";
    else if (k == TokenKind_newline)
        return "<new-line>";
    else if (k == TokenKind_other)
        return "<other>";
    else if (k == TokenKind_character_constant)
        return "<character-constant>";
    else if (k == TokenKind_header_name)
        return "<header-name>";
    else if (k == TokenKind_pp_directive_define)
        return "#define";
    else if (k == TokenKind_pp_directive_elif)
        return "#elif";
    else if (k == TokenKind_pp_directive_elifdef)
        return "#elifdef";
    else if (k == TokenKind_pp_directive_elifndef)
        return "#elifndef";
    else if (k == TokenKind_pp_directive_else)
        return "#else";
    else if (k == TokenKind_pp_directive_embed)
        return "#embed";
    else if (k == TokenKind_pp_directive_endif)
        return "#endif";
    else if (k == TokenKind_pp_directive_error)
        return "#error";
    else if (k == TokenKind_pp_directive_if)
        return "#if";
    else if (k == TokenKind_pp_directive_ifdef)
        return "#ifdef";
    else if (k == TokenKind_pp_directive_ifndef)
        return "#ifndef";
    else if (k == TokenKind_pp_directive_include)
        return "#include";
    else if (k == TokenKind_pp_directive_line)
        return "#line";
    else if (k == TokenKind_pp_directive_pragma)
        return "#pragma";
    else if (k == TokenKind_pp_directive_undef)
        return "#undef";
    else if (k == TokenKind_pp_directive_warning)
        return "#warning";
    else if (k == TokenKind_pp_operator_defined)
        return "defined";
    else if (k == TokenKind_pp_operator___has_c_attribute)
        return "__has_c_attribute";
    else if (k == TokenKind_pp_operator___has_embed)
        return "__has_embed";
    else if (k == TokenKind_pp_operator___has_include)
        return "__has_include";
    else if (k == TokenKind_keyword_alignas)
        return "alignas";
    else if (k == TokenKind_keyword_alignof)
        return "alignof";
    else if (k == TokenKind_keyword_auto)
        return "auto";
    else if (k == TokenKind_keyword_bool)
        return "bool";
    else if (k == TokenKind_keyword_break)
        return "break";
    else if (k == TokenKind_keyword_case)
        return "case";
    else if (k == TokenKind_keyword_char)
        return "char";
    else if (k == TokenKind_keyword_const)
        return "const";
    else if (k == TokenKind_keyword_constexpr)
        return "constexpr";
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
    else if (k == TokenKind_keyword_false)
        return "false";
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
    else if (k == TokenKind_keyword_nullptr)
        return "nullptr";
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
    else if (k == TokenKind_keyword_static_assert)
        return "static_assert";
    else if (k == TokenKind_keyword_struct)
        return "struct";
    else if (k == TokenKind_keyword_switch)
        return "switch";
    else if (k == TokenKind_keyword_thread_local)
        return "thread_local";
    else if (k == TokenKind_keyword_true)
        return "true";
    else if (k == TokenKind_keyword_typedef)
        return "typedef";
    else if (k == TokenKind_keyword_typeof)
        return "typeof";
    else if (k == TokenKind_keyword_typeof_unqual)
        return "typeof_unqual";
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
    else if (k == TokenKind_keyword__Atomic)
        return "_Atomic";
    else if (k == TokenKind_keyword__BitInt)
        return "_BitInt";
    else if (k == TokenKind_keyword__Complex)
        return "_Complex";
    else if (k == TokenKind_keyword__Decimal128)
        return "_Decimal128";
    else if (k == TokenKind_keyword__Decimal32)
        return "_Decimal32";
    else if (k == TokenKind_keyword__Decimal64)
        return "_Decimal64";
    else if (k == TokenKind_keyword__Generic)
        return "_Generic";
    else if (k == TokenKind_keyword__Imaginary)
        return "_Imaginary";
    else if (k == TokenKind_keyword__Noreturn)
        return "_Noreturn";
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
    else
        unreachable();
}

const char* token_stringify(Token* t) {
    TokenKind k = t->kind;
    if (k == TokenKind_literal_int) {
        const char* kind_str = token_kind_stringify(k);
        char* buf = calloc(10 + strlen(kind_str) + 3 + 1, sizeof(char));
        sprintf(buf, "%d (%s)", t->value.integer, kind_str);
        return buf;
    } else if (k == TokenKind_other || k == TokenKind_character_constant || k == TokenKind_ident ||
               k == TokenKind_literal_int || k == TokenKind_literal_str) {
        const char* kind_str = token_kind_stringify(k);
        char* buf = calloc(strlen(t->value.string) + strlen(kind_str) + 3 + 1, sizeof(char));
        sprintf(buf, "%s (%s)", t->value.string, kind_str);
        return buf;
    } else {
        return token_kind_stringify(k);
    }
}

void tokens_init(TokenArray* tokens, size_t capacity) {
    tokens->len = 0;
    tokens->capacity = capacity;
    tokens->data = calloc(tokens->capacity, sizeof(Token));
}

void tokens_reserve(TokenArray* tokens, size_t size) {
    if (size <= tokens->capacity)
        return;
    while (tokens->capacity < size) {
        tokens->capacity *= 2;
    }
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
    MacroKind_undef,
    MacroKind_obj,
    MacroKind_func,
    MacroKind_builtin_file,
    MacroKind_builtin_line,
};
typedef enum MacroKind MacroKind;

const char* macro_kind_stringify(MacroKind kind) {
    if (kind == MacroKind_undef)
        return "undef";
    else if (kind == MacroKind_obj)
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
    const char* name;
    TokenArray parameters;
    TokenArray replacements;
};
typedef struct Macro Macro;

static int macro_find_param(Macro* macro, Token* tok) {
    if (tok->kind != TokenKind_ident)
        return -1;

    for (int i = 0; i < macro->parameters.len; ++i) {
        if (strcmp(macro->parameters.data[i].value.string, tok->value.string) == 0) {
            return i;
        }
    }
    return -1;
}

struct MacroArray {
    size_t len;
    size_t capacity;
    Macro* data;
};
typedef struct MacroArray MacroArray;

static MacroArray* macros_new() {
    MacroArray* macros = calloc(1, sizeof(MacroArray));
    macros->len = 0;
    macros->capacity = 8;
    macros->data = calloc(macros->capacity, sizeof(Macro));
    return macros;
}

static void macros_reserve(MacroArray* macros, size_t size) {
    if (size <= macros->capacity)
        return;
    while (macros->capacity < size) {
        macros->capacity *= 2;
    }
    macros->data = realloc(macros->data, macros->capacity * sizeof(Macro));
    memset(macros->data + macros->len, 0, (macros->capacity - macros->len) * sizeof(Macro));
}

static Macro* macros_push_new(MacroArray* macros) {
    macros_reserve(macros, macros->len + 1);
    return &macros->data[macros->len++];
}

static void macros_dump(MacroArray* macros) {
    fprintf(stderr, "MacroArray {\n");
    fprintf(stderr, "  len = %zu\n", macros->len);
    fprintf(stderr, "  data = [\n");
    for (int i = 0; i < macros->len; ++i) {
        Macro* m = &macros->data[i];
        fprintf(stderr, "    Macro {\n");
        fprintf(stderr, "      kind = %s\n", macro_kind_stringify(m->kind));
        fprintf(stderr, "      name = %s\n", m->name);
        fprintf(stderr, "      replacements = TODO\n");
        fprintf(stderr, "    }\n");
    }
    fprintf(stderr, "  ]\n");
    fprintf(stderr, "}\n");
}

static void add_predefined_macros(MacroArray* macros) {
    Macro* m;

    m = macros_push_new(macros);
    m->kind = MacroKind_obj;
    m->name = "__ducc__";
    tokens_init(&m->replacements, 1);
    Token* tok = tokens_push_new(&m->replacements);
    tok->kind = TokenKind_literal_int;
    tok->value.integer = 1;

    m = macros_push_new(macros);
    m->kind = MacroKind_builtin_file;
    m->name = "__FILE__";

    m = macros_push_new(macros);
    m->kind = MacroKind_builtin_line;
    m->name = "__LINE__";
}

struct MacroArg {
    TokenArray tokens;
};
typedef struct MacroArg MacroArg;

struct MacroArgArray {
    size_t len;
    size_t capacity;
    MacroArg* data;
};
typedef struct MacroArgArray MacroArgArray;

static MacroArgArray* macroargs_new() {
    MacroArgArray* macroargs = calloc(1, sizeof(MacroArgArray));
    macroargs->len = 0;
    macroargs->capacity = 2;
    macroargs->data = calloc(macroargs->capacity, sizeof(MacroArg));
    return macroargs;
}

static void macroargs_reserve(MacroArgArray* macroargs, size_t size) {
    if (size <= macroargs->capacity)
        return;
    while (macroargs->capacity < size) {
        macroargs->capacity *= 2;
    }
    macroargs->data = realloc(macroargs->data, macroargs->capacity * sizeof(MacroArg));
    memset(macroargs->data + macroargs->len, 0, (macroargs->capacity - macroargs->len) * sizeof(MacroArg));
}

static MacroArg* macroargs_push_new(MacroArgArray* macroargs) {
    macroargs_reserve(macroargs, macroargs->len + 1);
    return &macroargs->data[macroargs->len++];
}

struct PpLexer {
    InFile* src;
    BOOL at_bol;
    BOOL expect_header_name;
    TokenArray* pp_tokens;
};
typedef struct PpLexer PpLexer;

static PpLexer* pplexer_new(InFile* src) {
    PpLexer* ppl = calloc(1, sizeof(PpLexer));

    ppl->src = src;
    ppl->at_bol = TRUE;
    ppl->expect_header_name = FALSE;
    ppl->pp_tokens = calloc(1, sizeof(TokenArray));
    tokens_init(ppl->pp_tokens, 1024 * 16);

    return ppl;
}

static TokenKind pplexer_tokenize_pp_directive(PpLexer* ppl) {
    // Skip whitespaces after '#'.
    char c;
    while (isspace((c = infile_peek_char(ppl->src)))) {
        if (c == '\n')
            break;
        infile_next_char(ppl->src);
    }

    SourceLocation pp_directive_name_start_loc = ppl->src->loc;

    StrBuilder builder;
    strbuilder_init(&builder);
    while (isalnum(infile_peek_char(ppl->src))) {
        strbuilder_append_char(&builder, infile_peek_char(ppl->src));
        infile_next_char(ppl->src);
    }
    const char* pp_directive_name = builder.buf;

    if (builder.len == 0) {
        return TokenKind_hash;
    } else if (strcmp(pp_directive_name, "define") == 0) {
        return TokenKind_pp_directive_define;
    } else if (strcmp(pp_directive_name, "elif") == 0) {
        return TokenKind_pp_directive_elif;
    } else if (strcmp(pp_directive_name, "elifdef") == 0) {
        return TokenKind_pp_directive_elifdef;
    } else if (strcmp(pp_directive_name, "elifndef") == 0) {
        return TokenKind_pp_directive_elifndef;
    } else if (strcmp(pp_directive_name, "else") == 0) {
        return TokenKind_pp_directive_else;
    } else if (strcmp(pp_directive_name, "embed") == 0) {
        return TokenKind_pp_directive_embed;
    } else if (strcmp(pp_directive_name, "endif") == 0) {
        return TokenKind_pp_directive_endif;
    } else if (strcmp(pp_directive_name, "error") == 0) {
        return TokenKind_pp_directive_error;
    } else if (strcmp(pp_directive_name, "if") == 0) {
        return TokenKind_pp_directive_if;
    } else if (strcmp(pp_directive_name, "ifdef") == 0) {
        return TokenKind_pp_directive_ifdef;
    } else if (strcmp(pp_directive_name, "ifndef") == 0) {
        return TokenKind_pp_directive_ifndef;
    } else if (strcmp(pp_directive_name, "include") == 0) {
        ppl->expect_header_name = TRUE;
        return TokenKind_pp_directive_include;
    } else if (strcmp(pp_directive_name, "line") == 0) {
        return TokenKind_pp_directive_line;
    } else if (strcmp(pp_directive_name, "pragma") == 0) {
        return TokenKind_pp_directive_pragma;
    } else if (strcmp(pp_directive_name, "undef") == 0) {
        return TokenKind_pp_directive_undef;
    } else if (strcmp(pp_directive_name, "warning") == 0) {
        return TokenKind_pp_directive_warning;
    } else {
        fatal_error("%s:%d: unknown preprocessor directive (%s)", pp_directive_name_start_loc.filename,
                    pp_directive_name_start_loc.line, pp_directive_name);
    }
}

static void pplexer_tokenize_all(PpLexer* ppl) {
    while (!infile_eof(ppl->src)) {
        Token* tok = tokens_push_new(ppl->pp_tokens);
        tok->loc = ppl->src->loc;
        char c = infile_peek_char(ppl->src);

        if (ppl->expect_header_name && c == '"') {
            infile_next_char(ppl->src);
            StrBuilder builder;
            strbuilder_init(&builder);
            strbuilder_append_char(&builder, '"');
            while (1) {
                char ch = infile_peek_char(ppl->src);
                if (ch == '"')
                    break;
                strbuilder_append_char(&builder, ch);
                if (ch == '\\') {
                    infile_next_char(ppl->src);
                    strbuilder_append_char(&builder, infile_peek_char(ppl->src));
                }
                infile_next_char(ppl->src);
            }
            strbuilder_append_char(&builder, '"');
            infile_next_char(ppl->src);
            tok->kind = TokenKind_header_name;
            tok->value.string = builder.buf;
            ppl->expect_header_name = FALSE;
        } else if (ppl->expect_header_name && c == '<') {
            infile_next_char(ppl->src);
            StrBuilder builder;
            strbuilder_init(&builder);
            strbuilder_append_char(&builder, '<');
            while (1) {
                char ch = infile_peek_char(ppl->src);
                if (ch == '>')
                    break;
                strbuilder_append_char(&builder, ch);
                infile_next_char(ppl->src);
            }
            strbuilder_append_char(&builder, '>');
            infile_next_char(ppl->src);
            tok->kind = TokenKind_header_name;
            tok->value.string = builder.buf;
            ppl->expect_header_name = FALSE;
        } else if (c == '(') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_paren_l;
        } else if (c == ')') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_paren_r;
        } else if (c == '{') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_brace_l;
        } else if (c == '}') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_brace_r;
        } else if (c == '[') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_bracket_l;
        } else if (c == ']') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_bracket_r;
        } else if (c == ',') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_comma;
        } else if (c == ':') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_colon;
        } else if (c == ';') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_semicolon;
        } else if (c == '^') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_assign_xor;
            } else {
                tok->kind = TokenKind_xor;
            }
        } else if (c == '?') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_question;
        } else if (c == '~') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_tilde;
        } else if (c == '+') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_assign_add;
            } else if (infile_consume_if(ppl->src, '+')) {
                tok->kind = TokenKind_plusplus;
            } else {
                tok->kind = TokenKind_plus;
            }
        } else if (c == '|') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_assign_or;
            } else if (infile_consume_if(ppl->src, '|')) {
                tok->kind = TokenKind_oror;
            } else {
                tok->kind = TokenKind_or;
            }
        } else if (c == '&') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_assign_and;
            } else if (infile_consume_if(ppl->src, '&')) {
                tok->kind = TokenKind_andand;
            } else {
                tok->kind = TokenKind_and;
            }
        } else if (c == '-') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '>')) {
                tok->kind = TokenKind_arrow;
            } else if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_assign_sub;
            } else if (infile_consume_if(ppl->src, '-')) {
                tok->kind = TokenKind_minusminus;
            } else {
                tok->kind = TokenKind_minus;
            }
        } else if (c == '*') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_assign_mul;
            } else {
                tok->kind = TokenKind_star;
            }
        } else if (c == '/') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_assign_div;
            } else if (infile_consume_if(ppl->src, '/')) {
                while (!infile_eof(ppl->src) && infile_peek_char(ppl->src) != '\n') {
                    infile_next_char(ppl->src);
                }
                tok->kind = TokenKind_whitespace;
            } else if (infile_consume_if(ppl->src, '*')) {
                while (infile_peek_char(ppl->src)) {
                    if (infile_consume_if(ppl->src, '*')) {
                        if (infile_consume_if(ppl->src, '/')) {
                            break;
                        }
                        continue;
                    }
                    infile_next_char(ppl->src);
                }
                tok->kind = TokenKind_whitespace;
            } else {
                tok->kind = TokenKind_slash;
            }
        } else if (c == '%') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_assign_mod;
            } else {
                tok->kind = TokenKind_percent;
            }
        } else if (c == '.') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '.')) {
                if (infile_consume_if(ppl->src, '.')) {
                    tok->kind = TokenKind_ellipsis;
                } else {
                    tok->kind = TokenKind_other;
                    tok->value.string = "..";
                }
            } else {
                tok->kind = TokenKind_dot;
            }
        } else if (c == '!') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_ne;
            } else {
                tok->kind = TokenKind_not;
            }
        } else if (c == '=') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_eq;
            } else {
                tok->kind = TokenKind_assign;
            }
        } else if (c == '<') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_le;
            } else if (infile_consume_if(ppl->src, '<')) {
                if (infile_consume_if(ppl->src, '=')) {
                    tok->kind = TokenKind_assign_lshift;
                } else {
                    tok->kind = TokenKind_lshift;
                }
            } else {
                tok->kind = TokenKind_lt;
            }
        } else if (c == '>') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '=')) {
                tok->kind = TokenKind_ge;
            } else if (infile_consume_if(ppl->src, '>')) {
                if (infile_consume_if(ppl->src, '=')) {
                    tok->kind = TokenKind_assign_rshift;
                } else {
                    tok->kind = TokenKind_rshift;
                }
            } else {
                tok->kind = TokenKind_gt;
            }
        } else if (c == '#') {
            infile_next_char(ppl->src);
            if (infile_consume_if(ppl->src, '#')) {
                tok->kind = TokenKind_hashhash;
            } else {
                tok->kind = ppl->at_bol ? pplexer_tokenize_pp_directive(ppl) : TokenKind_hash;
            }
        } else if (c == '\'') {
            infile_next_char(ppl->src);
            StrBuilder builder;
            strbuilder_init(&builder);
            strbuilder_append_char(&builder, '\'');
            strbuilder_append_char(&builder, infile_peek_char(ppl->src));
            if (infile_peek_char(ppl->src) == '\\') {
                infile_next_char(ppl->src);
                strbuilder_append_char(&builder, infile_peek_char(ppl->src));
            }
            strbuilder_append_char(&builder, '\'');
            infile_next_char(ppl->src);
            infile_next_char(ppl->src);
            tok->kind = TokenKind_character_constant;
            tok->value.string = builder.buf;
        } else if (c == '"') {
            infile_next_char(ppl->src);
            StrBuilder builder;
            strbuilder_init(&builder);
            while (1) {
                char ch = infile_peek_char(ppl->src);
                if (ch == '"')
                    break;
                strbuilder_append_char(&builder, ch);
                if (ch == '\\') {
                    infile_next_char(ppl->src);
                    strbuilder_append_char(&builder, infile_peek_char(ppl->src));
                }
                infile_next_char(ppl->src);
            }
            infile_next_char(ppl->src);
            tok->kind = TokenKind_literal_str;
            tok->value.string = builder.buf;
        } else if (isdigit(c)) {
            StrBuilder builder;
            strbuilder_init(&builder);
            while (isdigit(infile_peek_char(ppl->src))) {
                strbuilder_append_char(&builder, infile_peek_char(ppl->src));
                infile_next_char(ppl->src);
            }
            tok->kind = TokenKind_literal_int;
            tok->value.integer = atoi(builder.buf);
        } else if (isalpha(c) || c == '_') {
            StrBuilder builder;
            strbuilder_init(&builder);
            while (isalnum(infile_peek_char(ppl->src)) || infile_peek_char(ppl->src) == '_') {
                strbuilder_append_char(&builder, infile_peek_char(ppl->src));
                infile_next_char(ppl->src);
            }
            tok->kind = TokenKind_ident;
            tok->value.string = builder.buf;
        } else if (c == '\n') {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_newline;
        } else if (isspace(c)) {
            while (isspace((c = infile_peek_char(ppl->src)))) {
                if (c == '\n')
                    break;
                infile_next_char(ppl->src);
            }
            if (ppl->at_bol && infile_peek_char(ppl->src) == '#') {
                infile_next_char(ppl->src);
                tok->kind = pplexer_tokenize_pp_directive(ppl);
            } else {
                tok->kind = TokenKind_whitespace;
            }
        } else {
            infile_next_char(ppl->src);
            tok->kind = TokenKind_other;
            char* buf = calloc(2, sizeof(char));
            buf[0] = c;
            tok->value.string = buf;
        }
        ppl->at_bol = tok->kind == TokenKind_newline;
    }
    Token* eof_tok = tokens_push_new(ppl->pp_tokens);
    eof_tok->loc = ppl->src->loc;
    eof_tok->kind = TokenKind_eof;
}

static TokenArray* pp_tokenize(InFile* src) {
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
    char** include_paths;
    int n_include_paths;
};
typedef struct Preprocessor Preprocessor;

static TokenArray* do_preprocess(InFile* src, int depth, MacroArray* macros);

static Preprocessor* preprocessor_new(TokenArray* pp_tokens, int include_depth, MacroArray* macros) {
    if (include_depth >= 32) {
        fatal_error("include depth limit exceeded");
    }

    Preprocessor* pp = calloc(1, sizeof(Preprocessor));
    pp->pp_tokens = pp_tokens;
    pp->macros = macros;
    pp->include_depth = include_depth;
    pp->include_paths = calloc(16, sizeof(char*));

    return pp;
}

static Token* pp_token_at(Preprocessor* pp, int i) {
    return &pp->pp_tokens->data[i];
}

static Token* peek_pp_token(Preprocessor* pp) {
    return pp_token_at(pp, pp->pos);
}

static Token* next_pp_token(Preprocessor* pp) {
    return pp_token_at(pp, pp->pos++);
}

static BOOL pp_eof(Preprocessor* pp) {
    return peek_pp_token(pp)->kind == TokenKind_eof;
}

static int find_macro(Preprocessor* pp, const char* name) {
    for (int i = 0; i < pp->macros->len; ++i) {
        if (pp->macros->data[i].kind == MacroKind_undef)
            continue;
        if (strcmp(pp->macros->data[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static void undef_macro(Preprocessor* pp, int idx) {
    pp->macros->data[idx].kind = MacroKind_undef;
    // TODO: Can predefined macro like __FILE__ be undefined?
}

static void add_include_path(Preprocessor* pp, char* include_path) {
    pp->include_paths[pp->n_include_paths] = include_path;
    ++pp->n_include_paths;
}

static BOOL skip_pp_tokens(Preprocessor* pp) {
    // TODO: support nested #if
    return pp->skip_pp_tokens;
}

static void skip_whitespaces(Preprocessor* pp) {
    while (!pp_eof(pp) && peek_pp_token(pp)->kind == TokenKind_whitespace) {
        next_pp_token(pp);
    }
}

static void seek_to_next_newline(Preprocessor* pp) {
    while (!pp_eof(pp)) {
        Token* tok = peek_pp_token(pp);
        if (tok->kind == TokenKind_newline) {
            break;
        }
        next_pp_token(pp);
    }
}

static void make_token_whitespace(Token* tok) {
    tok->kind = TokenKind_whitespace;
    tok->value.string = NULL;
}

static void remove_directive_tokens(Preprocessor* pp, int start, int end) {
    for (int i = start; i < end; ++i) {
        make_token_whitespace(pp_token_at(pp, i));
    }
}

static void process_endif_directive(Preprocessor* pp, int directive_token_pos) {
    next_pp_token(pp);
    pp->skip_pp_tokens = FALSE;
    remove_directive_tokens(pp, directive_token_pos, pp->pos);
}

static void process_else_directive(Preprocessor* pp, int directive_token_pos) {
    next_pp_token(pp);
    pp->skip_pp_tokens = !pp->skip_pp_tokens;
    remove_directive_tokens(pp, directive_token_pos, pp->pos);
}

static void process_elif_directive(Preprocessor* pp, int directive_token_pos) {
    unimplemented();
}

static int replace_pp_tokens(Preprocessor*, int, int, TokenArray*);
static BOOL expand_macro(Preprocessor*);

static void process_if_directive(Preprocessor* pp, int directive_token_pos) {
    next_pp_token(pp);
    int condition_expression_start_pos = pp->pos;

    while (!pp_eof(pp)) {
        Token* tok = peek_pp_token(pp);
        if (tok->kind == TokenKind_newline) {
            break;
        } else if (tok->kind == TokenKind_ident) {
            if (strcmp(tok->value.string, "defined") == 0) {
                int defined_pos = pp->pos;
                // 'defined' <ws>* '(' <ws>* <ident> <ws>* ')'
                // 'defined' <ws>* <ident>
                next_pp_token(pp);
                skip_whitespaces(pp);
                Token* macro_name;
                if (peek_pp_token(pp)->kind == TokenKind_paren_l) {
                    next_pp_token(pp);
                    skip_whitespaces(pp);
                    macro_name = next_pp_token(pp);
                    if (macro_name->kind != TokenKind_ident) {
                        fatal_error("invalid defined");
                    }
                    skip_whitespaces(pp);
                    if (next_pp_token(pp)->kind != TokenKind_paren_r) {
                        fatal_error("invalid defined");
                    }
                } else {
                    macro_name = next_pp_token(pp);
                    if (macro_name->kind != TokenKind_ident) {
                        fatal_error("invalid defined");
                    }
                }
                BOOL is_defined = find_macro(pp, macro_name->value.string) != -1;
                TokenArray defined_results;
                tokens_init(&defined_results, 1);
                Token* defined_result = tokens_push_new(&defined_results);
                defined_result->kind = TokenKind_literal_int;
                defined_result->value.integer = is_defined;
                pp->pos = replace_pp_tokens(pp, defined_pos, pp->pos, &defined_results);
            } else {
                BOOL expanded = expand_macro(pp);
                if (expanded) {
                    // A macro may expand to another macro. Re-scan the expanded tokens.
                    // TODO: if the macro is defined recursively, it causes infinite loop.
                } else {
                    next_pp_token(pp);
                }
            }
        } else {
            next_pp_token(pp);
        }
    }

    // all remaining identifiers other than true (including those lexically identical to keywords such as false) are
    // replaced with the pp-number 0, true is replaced with pp-number 1, and then each preprocessing token is converted
    // into a token.
    for (int pos = condition_expression_start_pos; pos < pp->pos; ++pos) {
        Token* tok = pp_token_at(pp, pos);
        if (tok->kind == TokenKind_ident) {
            BOOL is_true = strcmp(tok->value.string, "true") == 0;
            tok->kind = TokenKind_literal_int;
            tok->value.integer = is_true;
        }
    }

    int condition_expression_tokens_len = pp->pos - condition_expression_start_pos;
    TokenArray condition_expression_tokens;
    // +1 to add EOF token at the end.
    tokens_init(&condition_expression_tokens, condition_expression_tokens_len + 1);
    for (int i = 0; i < condition_expression_tokens_len; ++i) {
        *tokens_push_new(&condition_expression_tokens) = *pp_token_at(pp, condition_expression_start_pos + i);
    }
    Token* eof_tok = tokens_push_new(&condition_expression_tokens);
    eof_tok->kind = TokenKind_eof;

    BOOL result = pp_eval_constant_expression(&condition_expression_tokens);

    pp->skip_pp_tokens = !result;

    remove_directive_tokens(pp, directive_token_pos, pp->pos);
}

static void process_ifdef_directive(Preprocessor* pp, int directive_token_pos) {
    next_pp_token(pp);
    skip_whitespaces(pp);
    Token* macro_name = peek_pp_token(pp);
    if (macro_name->kind == TokenKind_ident) {
        next_pp_token(pp);
        pp->skip_pp_tokens = find_macro(pp, macro_name->value.string) == -1;
    }
    remove_directive_tokens(pp, directive_token_pos, pp->pos);
}

static void process_ifndef_directive(Preprocessor* pp, int directive_token_pos) {
    next_pp_token(pp);
    skip_whitespaces(pp);
    Token* macro_name = peek_pp_token(pp);
    if (macro_name->kind == TokenKind_ident) {
        next_pp_token(pp);
        pp->skip_pp_tokens = find_macro(pp, macro_name->value.string) != -1;
    }
    remove_directive_tokens(pp, directive_token_pos, pp->pos);
}

static Token* read_include_header_name(Preprocessor* pp) {
    Token* tok = next_pp_token(pp);
    if (tok->kind != TokenKind_header_name) {
        fatal_error("%s:%d: invalid #include", tok->loc.filename, tok->loc.line);
    }
    return tok;
}

static const char* resolve_include_name(Preprocessor* pp, const Token* include_name_token) {
    const char* include_name = include_name_token->value.string;
    if (include_name[0] == '"') {
        char* current_filename = strdup(include_name_token->loc.filename);
        const char* current_dir = dirname(current_filename);
        char* buf = calloc(strlen(include_name) - 2 + 1 + strlen(current_dir) + 1, sizeof(char));
        sprintf(buf, "%s/%.*s", current_dir, strlen(include_name) - 2, include_name + 1);
        return buf;
    } else {
        for (int i = 0; i < pp->n_include_paths; ++i) {
            char* buf = calloc(strlen(include_name) - 2 + 1 + strlen(pp->include_paths[i]) + 1, sizeof(char));
            sprintf(buf, "%s/%.*s", pp->include_paths[i], strlen(include_name) - 2, include_name + 1);
            if (access(buf, F_OK | R_OK) == 0) {
                return buf;
            }
        }
        return NULL;
    }
}

static int replace_pp_tokens(Preprocessor* pp, int dest_start, int dest_end, TokenArray* source_tokens) {
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

static int replace_single_pp_token(Preprocessor* pp, int dest, Token* source_tok) {
    TokenArray tokens;
    tokens_init(&tokens, 1);
    *tokens_push_new(&tokens) = *source_tok;
    replace_pp_tokens(pp, dest, dest + 1, &tokens);
}

static void expand_include_directive(Preprocessor* pp, int directive_token_pos, const char* include_name) {
    InFile* include_source = infile_open(include_name);
    if (!include_source) {
        fatal_error("cannot open include file: %s", include_name);
    }

    TokenArray* include_pp_tokens = do_preprocess(include_source, pp->include_depth + 1, pp->macros);
    tokens_pop(include_pp_tokens); // pop EOF token
    pp->pos = replace_pp_tokens(pp, directive_token_pos, pp->pos, include_pp_tokens);
}

static void process_include_directive(Preprocessor* pp, int directive_token_pos) {
    next_pp_token(pp);
    skip_whitespaces(pp);
    Token* include_name = read_include_header_name(pp);
    const char* include_name_resolved = resolve_include_name(pp, include_name);
    if (include_name_resolved == NULL) {
        fatal_error("cannot resolve include file name: %s", include_name);
    }
    expand_include_directive(pp, directive_token_pos, include_name_resolved);
}

// ws ::= many0(<whitespace>)
// macro-parameters ::= '(' <ws> opt(<identifier> <ws> many0(',' <ws> <identifier> <ws>)) ')'
static TokenArray* pp_parse_macro_parameters(Preprocessor* pp) {
    TokenArray* parameters = calloc(1, sizeof(TokenArray));
    tokens_init(parameters, 2);

    // '(' is consumed by caller.
    skip_whitespaces(pp);
    Token* tok = next_pp_token(pp);
    if (tok->kind == TokenKind_ident) {
        *tokens_push_new(parameters) = *tok;
        skip_whitespaces(pp);
        while (peek_pp_token(pp)->kind == TokenKind_comma) {
            next_pp_token(pp);
            skip_whitespaces(pp);
            tok = next_pp_token(pp);
            if (tok->kind != TokenKind_ident) {
                fatal_error("%s:%d: invalid macro syntax", tok->loc.filename, tok->loc.line);
            }
            *tokens_push_new(parameters) = *tok;
        }
        tok = next_pp_token(pp);
    }
    if (tok->kind != TokenKind_paren_r) {
        fatal_error("%s:%d: invalid macro syntax", tok->loc.filename, tok->loc.line);
    }

    return parameters;
}

static void process_define_directive(Preprocessor* pp, int directive_token_pos) {
    next_pp_token(pp);
    skip_whitespaces(pp);
    Token* macro_name = next_pp_token(pp);

    if (macro_name->kind != TokenKind_ident) {
        fatal_error("%s:%d: invalid #define syntax", macro_name->loc.filename, macro_name->loc.line);
    }

    if (peek_pp_token(pp)->kind == TokenKind_paren_l) {
        next_pp_token(pp);
        TokenArray* parameters = pp_parse_macro_parameters(pp);
        int replacements_start_pos = pp->pos;
        seek_to_next_newline(pp);
        if (pp_eof(pp)) {
            fatal_error("%s:%d: invalid #define syntax", macro_name->loc.filename, macro_name->loc.line);
        }
        Macro* macro = macros_push_new(pp->macros);
        macro->kind = MacroKind_func;
        macro->name = macro_name->value.string;
        macro->parameters = *parameters;
        int n_replacements = pp->pos - replacements_start_pos;
        tokens_init(&macro->replacements, n_replacements);
        for (int i = 0; i < n_replacements; ++i) {
            *tokens_push_new(&macro->replacements) = *pp_token_at(pp, replacements_start_pos + i);
        }
    } else {
        int replacements_start_pos = pp->pos;
        seek_to_next_newline(pp);
        if (pp_eof(pp)) {
            fatal_error("%s:%d: invalid #define syntax", macro_name->loc.filename, macro_name->loc.line);
        }
        Macro* macro = macros_push_new(pp->macros);
        macro->kind = MacroKind_obj;
        macro->name = macro_name->value.string;
        int n_replacements = pp->pos - replacements_start_pos;
        tokens_init(&macro->replacements, n_replacements);
        for (int i = 0; i < n_replacements; ++i) {
            *tokens_push_new(&macro->replacements) = *pp_token_at(pp, replacements_start_pos + i);
        }
    }
    remove_directive_tokens(pp, directive_token_pos, pp->pos);
}

static void process_undef_directive(Preprocessor* pp, int directive_token_pos) {
    next_pp_token(pp);
    skip_whitespaces(pp);
    Token* macro_name = peek_pp_token(pp);
    if (macro_name->kind == TokenKind_ident) {
        next_pp_token(pp);
        int macro_idx = find_macro(pp, macro_name->value.string);
        if (macro_idx != -1) {
            undef_macro(pp, macro_idx);
        }
    }
    remove_directive_tokens(pp, directive_token_pos, pp->pos);
}

static void process_line_directive(Preprocessor* pp, int directive_token_pos) {
    unimplemented();
}

static void process_error_directive(Preprocessor* pp, int directive_token_pos) {
    unimplemented();
}

static void process_pragma_directive(Preprocessor* pp, int directive_token_pos) {
    unimplemented();
}

// ws ::= many0(<Whitespace>)
// macro-arguments ::= '(' <ws> opt(<any-token> <ws> many0(',' <ws> <any-token> <ws>)) ')'
static MacroArgArray* pp_parse_macro_arguments(Preprocessor* pp) {
    MacroArgArray* args = macroargs_new();

    Token* tok = next_pp_token(pp);
    if (tok->kind != TokenKind_paren_l) {
        fatal_error("%s:%d: invalid macro syntax", tok->loc.filename, tok->loc.line);
    }
    skip_whitespaces(pp);
    tok = next_pp_token(pp);
    if (tok->kind != TokenKind_paren_r) {
        MacroArg* arg = macroargs_push_new(args);
        tokens_init(&arg->tokens, 1);
        *tokens_push_new(&arg->tokens) = *tok;
        skip_whitespaces(pp);
        while (peek_pp_token(pp)->kind == TokenKind_comma) {
            next_pp_token(pp);
            skip_whitespaces(pp);
            tok = next_pp_token(pp);
            arg = macroargs_push_new(args);
            tokens_init(&arg->tokens, 1);
            *tokens_push_new(&arg->tokens) = *tok;
        }
        tok = next_pp_token(pp);
    }
    if (tok->kind != TokenKind_paren_r) {
        fatal_error("%s:%d: invalid macro syntax", tok->loc.filename, tok->loc.line);
    }

    return args;
}

static BOOL expand_macro(Preprocessor* pp) {
    int macro_name_pos = pp->pos;
    Token* macro_name = next_pp_token(pp);
    int macro_idx = find_macro(pp, macro_name->value.string);
    if (macro_idx == -1) {
        return FALSE;
    }

    SourceLocation original_loc = macro_name->loc;
    Macro* macro = &pp->macros->data[macro_idx];
    if (macro->kind == MacroKind_func) {
        MacroArgArray* args = pp_parse_macro_arguments(pp);
        replace_pp_tokens(pp, macro_name_pos, pp->pos, &macro->replacements);
        for (int i = 0; i < macro->replacements.len; ++i) {
            Token* tok = pp_token_at(pp, macro_name_pos + i);
            int macro_param_idx = macro_find_param(macro, tok);
            if (macro_param_idx != -1) {
                replace_pp_tokens(pp, macro_name_pos + i, macro_name_pos + i + 1, &args->data[macro_param_idx].tokens);
            }
        }
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
        Token file_tok;
        file_tok.kind = TokenKind_literal_str;
        file_tok.value.string = macro_name->loc.filename;
        file_tok.loc.filename = NULL;
        file_tok.loc.line = 0;
        replace_single_pp_token(pp, macro_name_pos, &file_tok);
    } else if (macro->kind == MacroKind_builtin_line) {
        Token line_tok;
        line_tok.kind = TokenKind_literal_int;
        line_tok.value.integer = macro_name->loc.line;
        line_tok.loc.filename = NULL;
        line_tok.loc.line = 0;
        replace_single_pp_token(pp, macro_name_pos, &line_tok);
    } else {
        unreachable();
    }
    return TRUE;
}

static void process_pp_directive(Preprocessor* pp) {
    int first_token_pos = pp->pos;
    Token* tok = peek_pp_token(pp);
    if (tok->kind == TokenKind_pp_directive_endif) {
        process_endif_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_pp_directive_else) {
        process_else_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_pp_directive_elif) {
        process_elif_directive(pp, first_token_pos);
    } else if (skip_pp_tokens(pp)) {
        make_token_whitespace(next_pp_token(pp));
    } else if (tok->kind == TokenKind_pp_directive_if) {
        process_if_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_pp_directive_ifdef) {
        process_ifdef_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_pp_directive_ifndef) {
        process_ifndef_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_pp_directive_include) {
        process_include_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_pp_directive_define) {
        process_define_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_pp_directive_undef) {
        process_undef_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_pp_directive_line) {
        process_line_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_pp_directive_error) {
        process_error_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_pp_directive_pragma) {
        process_pragma_directive(pp, first_token_pos);
    } else if (tok->kind == TokenKind_ident) {
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

static void process_pp_directives(Preprocessor* pp) {
    while (!pp_eof(pp)) {
        process_pp_directive(pp);
    }
}

static void pp_dump(Token* t, BOOL include_whitespace) {
    for (; t->kind != TokenKind_eof; ++t) {
        if (t->kind == TokenKind_whitespace && !include_whitespace) {
            continue;
        }
        fprintf(stderr, "%s\n", token_stringify(t));
    }
}

static char* get_ducc_include_path() {
    const char* self_dir = get_self_dir();
    char* buf = calloc(strlen(self_dir) + strlen("/../include") + 1, sizeof(char));
    sprintf(buf, "%s/../include", self_dir);
    return buf;
}

static TokenArray* do_preprocess(InFile* src, int depth, MacroArray* macros) {
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
