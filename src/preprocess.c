#include "preprocess.h"
#include "common.h"
#include "parse.h"
#include "sys.h"

typedef enum {
    MacroKind_undef,
    MacroKind_obj,
    MacroKind_func,
    MacroKind_builtin_file,
    MacroKind_builtin_line,
} MacroKind;

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

typedef struct {
    MacroKind kind;
    const char* name;
    TokenArray parameters;
    TokenArray replacements;
} Macro;

static int macro_find_param(Macro* macro, Token* tok) {
    if (tok->kind != TokenKind_ident)
        return -1;

    for (size_t i = 0; i < macro->parameters.len; ++i) {
        if (strcmp(macro->parameters.data[i].value.string, tok->value.string) == 0) {
            return i;
        }
    }
    return -1;
}

typedef struct {
    size_t len;
    size_t capacity;
    Macro* data;
} MacroArray;

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

static void define_macro_to_1(MacroArray* macros, const char* name) {
    Macro* m = macros_push_new(macros);
    m->kind = MacroKind_obj;
    m->name = name;
    tokens_init(&m->replacements, 1);
    Token* tok = tokens_push_new(&m->replacements);
    tok->kind = TokenKind_literal_int;
    tok->value.integer = 1;
}

static void add_predefined_macros(MacroArray* macros) {
    Macro* m;

    m = macros_push_new(macros);
    m->kind = MacroKind_builtin_file;
    m->name = "__FILE__";

    m = macros_push_new(macros);
    m->kind = MacroKind_builtin_line;
    m->name = "__LINE__";

    // Non-standard pre-defined macros.
    define_macro_to_1(macros, "__ducc__");
    define_macro_to_1(macros, "__x86_64__");
    define_macro_to_1(macros, "__LP64__");
}

typedef struct {
    TokenArray tokens;
} MacroArg;

void macroarg_build_json(JsonBuilder* builder, MacroArg* arg) {
    jsonbuilder_object_start(builder);
    jsonbuilder_object_member_start(builder, "tokens");
    tokens_build_json(builder, &arg->tokens);
    jsonbuilder_object_member_end(builder);
    jsonbuilder_object_end(builder);
}

typedef struct {
    size_t len;
    size_t capacity;
    MacroArg* data;
} MacroArgArray;

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

void macroargs_build_json(JsonBuilder* builder, MacroArgArray* macroargs) {
    jsonbuilder_object_start(builder);
    jsonbuilder_object_member_start(builder, "len");
    jsonbuilder_integer(builder, macroargs->len);
    jsonbuilder_object_member_end(builder);
    jsonbuilder_object_member_start(builder, "data");
    jsonbuilder_array_start(builder);
    for (int i = 0; i < macroargs->len; ++i) {
        jsonbuilder_array_element_start(builder);
        macroarg_build_json(builder, &macroargs->data[i]);
        jsonbuilder_array_element_end(builder);
    }
    jsonbuilder_array_end(builder);
    jsonbuilder_object_member_end(builder);
    jsonbuilder_object_end(builder);
}

typedef struct {
    InFile* src;
    BOOL at_bol;
    BOOL expect_header_name;
    TokenArray* pp_tokens;
} PpLexer;

static PpLexer* pplexer_new(InFile* src) {
    PpLexer* ppl = calloc(1, sizeof(PpLexer));

    ppl->src = src;
    ppl->at_bol = TRUE;
    ppl->expect_header_name = FALSE;
    ppl->pp_tokens = calloc(1, sizeof(TokenArray));
    tokens_init(ppl->pp_tokens, 1024 * 16);

    return ppl;
}

static void pplexer_tokenize_pp_directive(PpLexer* ppl, Token* tok) {
    // Skip whitespaces after '#'.
    char c;
    while (isspace((c = infile_peek_char(ppl->src)))) {
        if (c == '\n')
            break;
        infile_next_char(ppl->src);
    }
    // '#' new-line
    if (c == '\n') {
        tok->kind = TokenKind_pp_directive_nop;
        return;
    }

    StrBuilder builder;
    strbuilder_init(&builder);
    while (isalnum(infile_peek_char(ppl->src))) {
        strbuilder_append_char(&builder, infile_peek_char(ppl->src));
        infile_next_char(ppl->src);
    }
    const char* pp_directive_name = builder.buf;

    if (builder.len == 0) {
        tok->kind = TokenKind_hash;
    } else if (strcmp(pp_directive_name, "define") == 0) {
        tok->kind = TokenKind_pp_directive_define;
    } else if (strcmp(pp_directive_name, "elif") == 0) {
        tok->kind = TokenKind_pp_directive_elif;
    } else if (strcmp(pp_directive_name, "elifdef") == 0) {
        tok->kind = TokenKind_pp_directive_elifdef;
    } else if (strcmp(pp_directive_name, "elifndef") == 0) {
        tok->kind = TokenKind_pp_directive_elifndef;
    } else if (strcmp(pp_directive_name, "else") == 0) {
        tok->kind = TokenKind_pp_directive_else;
    } else if (strcmp(pp_directive_name, "embed") == 0) {
        tok->kind = TokenKind_pp_directive_embed;
    } else if (strcmp(pp_directive_name, "endif") == 0) {
        tok->kind = TokenKind_pp_directive_endif;
    } else if (strcmp(pp_directive_name, "error") == 0) {
        tok->kind = TokenKind_pp_directive_error;
    } else if (strcmp(pp_directive_name, "if") == 0) {
        tok->kind = TokenKind_pp_directive_if;
    } else if (strcmp(pp_directive_name, "ifdef") == 0) {
        tok->kind = TokenKind_pp_directive_ifdef;
    } else if (strcmp(pp_directive_name, "ifndef") == 0) {
        tok->kind = TokenKind_pp_directive_ifndef;
    } else if (strcmp(pp_directive_name, "include") == 0) {
        ppl->expect_header_name = TRUE;
        tok->kind = TokenKind_pp_directive_include;
    } else if (strcmp(pp_directive_name, "line") == 0) {
        tok->kind = TokenKind_pp_directive_line;
    } else if (strcmp(pp_directive_name, "pragma") == 0) {
        tok->kind = TokenKind_pp_directive_pragma;
    } else if (strcmp(pp_directive_name, "undef") == 0) {
        tok->kind = TokenKind_pp_directive_undef;
    } else if (strcmp(pp_directive_name, "warning") == 0) {
        tok->kind = TokenKind_pp_directive_warning;
    } else {
        tok->kind = TokenKind_pp_directive_non_directive;
        tok->value.string = pp_directive_name;
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
                if (ppl->at_bol) {
                    pplexer_tokenize_pp_directive(ppl, tok);
                } else {
                    tok->kind = TokenKind_hash;
                }
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
            // Skip integer suffix.
            while (isalpha(infile_peek_char(ppl->src))) {
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
                pplexer_tokenize_pp_directive(ppl, tok);
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

typedef struct {
    TokenArray* pp_tokens;
    int pos;
    MacroArray* macros;
    int include_depth;
    StrArray include_paths;
    StrArray* included_files;
} Preprocessor;

static TokenArray* do_preprocess(InFile* src, int depth, MacroArray* macros, StrArray* included_files);

static Preprocessor* preprocessor_new(TokenArray* pp_tokens, int include_depth, MacroArray* macros,
                                      StrArray* included_files) {
    if (include_depth >= 32) {
        fatal_error("include depth limit exceeded");
    }

    Preprocessor* pp = calloc(1, sizeof(Preprocessor));
    pp->pp_tokens = pp_tokens;
    pp->macros = macros;
    pp->include_depth = include_depth;
    strings_init(&pp->include_paths);
    pp->included_files = included_files;

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

static void skip_pp_token(Preprocessor* pp, TokenKind expected) {
    Token* tok = next_pp_token(pp);
    assert(tok->kind == expected);
}

static Token* consume_pp_token_if(Preprocessor* pp, TokenKind expected) {
    if (peek_pp_token(pp)->kind == expected) {
        return next_pp_token(pp);
    } else {
        return NULL;
    }
}

static Token* consume_pp_token_if_not(Preprocessor* pp, TokenKind non_expected) {
    if (peek_pp_token(pp)->kind == non_expected) {
        return NULL;
    } else {
        return next_pp_token(pp);
    }
}

static Token* expect_pp_token(Preprocessor* pp, TokenKind expected) {
    Token* tok = next_pp_token(pp);
    if (tok->kind == expected) {
        return tok;
    }
    fatal_error("%s:%d: expected '%s', but got '%s'", tok->loc.filename, tok->loc.line, token_kind_stringify(expected),
                token_stringify(tok));
}

static BOOL pp_eof(Preprocessor* pp) {
    return peek_pp_token(pp)->kind == TokenKind_eof;
}

static int find_macro(Preprocessor* pp, const char* name) {
    for (size_t i = 0; i < pp->macros->len; ++i) {
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

static void add_include_path(Preprocessor* pp, const char* include_path) {
    strings_push(&pp->include_paths, include_path);
}

static void skip_whitespaces(Preprocessor* pp) {
    while (!pp_eof(pp) && consume_pp_token_if(pp, TokenKind_whitespace))
        ;
}

static void skip_whitespaces_or_newlines(Preprocessor* pp, BOOL skip_newline) {
    while (!pp_eof(pp) && (consume_pp_token_if(pp, TokenKind_whitespace) ||
                           (skip_newline && consume_pp_token_if(pp, TokenKind_newline))))
        ;
}

// It will not consume a new-line token.
static void seek_to_next_newline(Preprocessor* pp) {
    while (!pp_eof(pp) && consume_pp_token_if_not(pp, TokenKind_newline))
        ;
}

static void make_tokens_whitespaces(Preprocessor* pp, int start, int end) {
    for (int i = start; i < end; ++i) {
        Token* tok = pp_token_at(pp, i);
        tok->kind = TokenKind_whitespace;
        tok->value.string = NULL;
    }
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
        sprintf(buf, "%s/%.*s", current_dir, (int)(strlen(include_name) - 2), include_name + 1);
        return buf;
    } else {
        for (size_t i = 0; i < pp->include_paths.len; ++i) {
            char* buf = calloc(strlen(include_name) - 2 + 1 + strlen(pp->include_paths.data[i]) + 1, sizeof(char));
            sprintf(buf, "%s/%.*s", pp->include_paths.data[i], (int)(strlen(include_name) - 2), include_name + 1);
            if (access(buf, F_OK | R_OK) == 0) {
                return buf;
            }
        }
        return NULL;
    }
}

static int replace_pp_tokens(Preprocessor* pp, int dest_start, int dest_end, TokenArray* source_tokens) {
    size_t n_tokens_to_remove = dest_end - dest_start;
    size_t n_tokens_after_dest = pp->pp_tokens->len - dest_end;
    size_t shift_amount;

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

static int insert_pp_tokens(Preprocessor* pp, int dest_pos, TokenArray* source_tokens) {
    return replace_pp_tokens(pp, dest_pos, dest_pos, source_tokens);
}

static void replace_single_pp_token(Preprocessor* pp, int dest, Token* source_tok) {
    TokenArray tokens;
    tokens_init(&tokens, 1);
    *tokens_push_new(&tokens) = *source_tok;
    replace_pp_tokens(pp, dest, dest + 1, &tokens);
}

static void expand_include_directive(Preprocessor* pp, const char* include_name, Token* original_include_name_tok) {
    InFile* include_source = infile_open(include_name);
    if (!include_source) {
        fatal_error("%s:%d: cannot open include file: %s", original_include_name_tok->loc.filename,
                    original_include_name_tok->loc.line, token_stringify(original_include_name_tok));
    }

    TokenArray* include_pp_tokens =
        do_preprocess(include_source, pp->include_depth + 1, pp->macros, pp->included_files);
    tokens_pop(include_pp_tokens); // pop EOF token
    pp->pos = insert_pp_tokens(pp, pp->pos, include_pp_tokens);
}

// ws ::= many0(<whitespace>)
// macro-parameters ::= '(' <ws> opt(<identifier> <ws> many0(',' <ws> <identifier> <ws>)) ')'
static TokenArray* pp_parse_macro_parameters(Preprocessor* pp) {
    TokenArray* parameters = calloc(1, sizeof(TokenArray));
    tokens_init(parameters, 2);

    // '(' is consumed by caller.
    skip_whitespaces(pp);
    Token* tok = consume_pp_token_if(pp, TokenKind_ident);
    if (tok) {
        *tokens_push_new(parameters) = *tok;
        skip_whitespaces(pp);
        while (consume_pp_token_if(pp, TokenKind_comma)) {
            skip_whitespaces(pp);
            tok = next_pp_token(pp);
            if (tok->kind != TokenKind_ident) {
                fatal_error("%s:%d: invalid macro syntax", tok->loc.filename, tok->loc.line);
            }
            *tokens_push_new(parameters) = *tok;
        }
    }
    expect_pp_token(pp, TokenKind_paren_r);

    return parameters;
}

// ws ::= many0(<whitespace>)
// macro-arguments ::= <ws> '(' <ws> opt(<any-token> <ws> many0(',' <ws> <any-token> <ws>)) ')'
static MacroArgArray* pp_parse_macro_arguments(Preprocessor* pp, BOOL skip_newline) {
    MacroArgArray* args = macroargs_new();

    skip_whitespaces_or_newlines(pp, skip_newline);
    expect_pp_token(pp, TokenKind_paren_l);
    skip_whitespaces_or_newlines(pp, skip_newline);
    Token* tok = peek_pp_token(pp);
    if (!skip_newline && tok->kind == TokenKind_newline) {
        expect_pp_token(pp, TokenKind_paren_r);
    }
    if (tok->kind != TokenKind_paren_r) {
        MacroArg* arg = macroargs_push_new(args);
        tokens_init(&arg->tokens, 4);

        // Parse argument tokens, handling nested parentheses.
        int nesting = 0;
        while (TRUE) {
            tok = peek_pp_token(pp);

            if (nesting == 0) {
                if (tok->kind == TokenKind_paren_r) {
                    break;
                }
                if (tok->kind == TokenKind_comma) {
                    next_pp_token(pp);
                    skip_whitespaces_or_newlines(pp, skip_newline);

                    arg = macroargs_push_new(args);
                    tokens_init(&arg->tokens, 4);
                    continue;
                }
            }

            if (tok->kind == TokenKind_paren_l) {
                nesting++;
            } else if (tok->kind == TokenKind_paren_r) {
                nesting--;
                if (nesting < 0) {
                    break;
                }
            }

            tok = next_pp_token(pp);
            *tokens_push_new(&arg->tokens) = *tok;

            skip_whitespaces_or_newlines(pp, skip_newline);
        }
    }
    expect_pp_token(pp, TokenKind_paren_r);

    return args;
}

static Token* concat_two_tokens(Token* left, Token* right) {
    StrBuilder builder;
    strbuilder_init(&builder);

    // Left
    if (left->kind == TokenKind_ident) {
        strbuilder_append_string(&builder, left->value.string);
    } else if (left->kind == TokenKind_literal_int) {
        char buf[32];
        sprintf(buf, "%d", left->value.integer);
        strbuilder_append_string(&builder, buf);
    } else {
        strbuilder_append_string(&builder, token_stringify(left));
    }

    // Right
    if (right->kind == TokenKind_ident) {
        strbuilder_append_string(&builder, right->value.string);
    } else if (right->kind == TokenKind_literal_int) {
        char buf[32];
        sprintf(buf, "%d", right->value.integer);
        strbuilder_append_string(&builder, buf);
    } else {
        strbuilder_append_string(&builder, token_stringify(right));
    }

    // Concat
    Token* result = calloc(1, sizeof(Token));

    char* endptr;
    int val = strtol(builder.buf, &endptr, 10);
    if (*endptr == '\0') {
        result->kind = TokenKind_literal_int;
        result->value.integer = val;
    } else {
        result->kind = TokenKind_ident;
        result->value.string = builder.buf;
    }

    return result;
}

static BOOL expand_macro(Preprocessor* pp, BOOL skip_newline) {
    int macro_name_pos = pp->pos;
    Token* macro_name = peek_pp_token(pp);
    int macro_idx = find_macro(pp, macro_name->value.string);
    if (macro_idx == -1) {
        return FALSE;
    }

    SourceLocation original_loc = macro_name->loc;
    Macro* macro = &pp->macros->data[macro_idx];
    if (macro->kind == MacroKind_func) {
        next_pp_token(pp);
        MacroArgArray* args = pp_parse_macro_arguments(pp, skip_newline);
        replace_pp_tokens(pp, macro_name_pos, pp->pos, &macro->replacements);

        // Parameter substitution
        size_t token_count = 0;
        size_t offset = 0;
        for (size_t i = 0; i < macro->replacements.len; ++i) {
            Token* tok = pp_token_at(pp, macro_name_pos + i + offset);
            int macro_param_idx = macro_find_param(macro, tok);
            if (macro_param_idx != -1) {
                size_t arg_token_count = args->data[macro_param_idx].tokens.len;
                replace_pp_tokens(pp, macro_name_pos + i + offset, macro_name_pos + i + offset + 1,
                                  &args->data[macro_param_idx].tokens);
                token_count += arg_token_count;
                offset += arg_token_count - 1;
            } else {
                ++token_count;
            }
        }

        // Handle ## operator
        size_t token_count2 = 0;
        for (size_t i = 0; i < token_count; ++i) {
            int pos = macro_name_pos + i;
            Token* tok = pp_token_at(pp, pos);
            if (tok->kind == TokenKind_hashhash) {
                // Concatenate previous and next tokens
                if (0 < i && i < token_count - 1) {
                    Token* left = pp_token_at(pp, pos - 1);
                    Token* right = pp_token_at(pp, pos + 1);
                    Token* concatenated = concat_two_tokens(left, right);

                    // Replace the three tokens (left ## right) with the concatenated one
                    TokenArray single_token;
                    tokens_init(&single_token, 1);
                    *tokens_push_new(&single_token) = *concatenated;
                    replace_pp_tokens(pp, pos - 1, pos + 2, &single_token);
                    --i;
                    ++token_count2;
                } else {
                    fatal_error("invalid usage of ## operator");
                }
            } else {
                ++token_count2;
            }
        }

        // Inherit a source location from the original macro token.
        for (size_t i = 0; i < token_count2; ++i) {
            pp_token_at(pp, macro_name_pos + i)->loc = original_loc;
        }
        pp->pos = macro_name_pos;
    } else if (macro->kind == MacroKind_obj) {
        replace_pp_tokens(pp, macro_name_pos, macro_name_pos + 1, &macro->replacements);
        // Inherit a source location from the original macro token.
        for (size_t i = 0; i < macro->replacements.len; ++i) {
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

typedef enum {
    GroupDelimiterKind_normal,
    GroupDelimiterKind_after_if_directive,
    GroupDelimiterKind_after_else_directive,
} GroupDelimiterKind;

static BOOL is_delimiter_of_current_group(GroupDelimiterKind delimiter_kind, TokenKind token_kind) {
    if (delimiter_kind == GroupDelimiterKind_normal) {
        return token_kind == TokenKind_eof;
    } else if (delimiter_kind == GroupDelimiterKind_after_if_directive) {
        return token_kind == TokenKind_pp_directive_elif || token_kind == TokenKind_pp_directive_elifdef ||
               token_kind == TokenKind_pp_directive_elifndef || token_kind == TokenKind_pp_directive_else ||
               token_kind == TokenKind_pp_directive_endif;
    } else if (delimiter_kind == GroupDelimiterKind_after_else_directive) {
        return token_kind == TokenKind_pp_directive_endif;
    } else {
        unreachable();
    }
}

static int replace_pp_tokens(Preprocessor*, int, int, TokenArray*);
static void include_conditionally(Preprocessor* pp, GroupDelimiterKind delimiter_kind, BOOL do_include);

static BOOL preprocess_if_group_or_elif_group(Preprocessor* pp, BOOL did_include) {
    Token* directive = next_pp_token(pp);

    if (directive->kind == TokenKind_pp_directive_if || directive->kind == TokenKind_pp_directive_elif) {
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
                    skip_pp_token(pp, TokenKind_ident);
                    skip_whitespaces(pp);
                    Token* macro_name;
                    if (consume_pp_token_if(pp, TokenKind_paren_l)) {
                        skip_whitespaces(pp);
                        macro_name = expect_pp_token(pp, TokenKind_ident);
                        skip_whitespaces(pp);
                        expect_pp_token(pp, TokenKind_paren_r);
                    } else {
                        macro_name = expect_pp_token(pp, TokenKind_ident);
                    }
                    BOOL is_defined = find_macro(pp, macro_name->value.string) != -1;
                    TokenArray defined_results;
                    tokens_init(&defined_results, 1);
                    Token* defined_result = tokens_push_new(&defined_results);
                    defined_result->kind = TokenKind_literal_int;
                    defined_result->value.integer = is_defined;
                    pp->pos = replace_pp_tokens(pp, defined_pos, pp->pos, &defined_results);
                } else {
                    BOOL expanded = expand_macro(pp, FALSE);
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
        // replaced with the pp-number 0, true is replaced with pp-number 1, and then each preprocessing token is
        // converted into a token.
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

        BOOL do_include = pp_eval_constant_expression(&condition_expression_tokens) && !did_include;
        include_conditionally(pp, GroupDelimiterKind_after_if_directive, do_include);
        return do_include;
    } else if (directive->kind == TokenKind_pp_directive_ifdef || directive->kind == TokenKind_pp_directive_elifdef) {
        skip_whitespaces(pp);
        Token* macro_name = consume_pp_token_if(pp, TokenKind_ident);
        if (!macro_name) {
            fatal_error("");
        }

        BOOL do_include = !did_include && find_macro(pp, macro_name->value.string) != -1;
        include_conditionally(pp, GroupDelimiterKind_after_if_directive, do_include);
        return do_include;
    } else if (directive->kind == TokenKind_pp_directive_ifndef || directive->kind == TokenKind_pp_directive_elifndef) {
        skip_whitespaces(pp);
        Token* macro_name = consume_pp_token_if(pp, TokenKind_ident);
        if (!macro_name) {
            fatal_error("");
        }

        BOOL do_include = !did_include && find_macro(pp, macro_name->value.string) == -1;
        include_conditionally(pp, GroupDelimiterKind_after_if_directive, do_include);
        return do_include;
    } else {
        unreachable();
    }
}

static BOOL preprocess_if_group(Preprocessor* pp) {
    return preprocess_if_group_or_elif_group(pp, FALSE);
}

static BOOL preprocess_elif_group(Preprocessor* pp, BOOL did_include) {
    return preprocess_if_group_or_elif_group(pp, did_include);
}

// elif-groups:
//     { elif-group }+
static BOOL preprocess_elif_groups_opt(Preprocessor* pp, BOOL did_include) {
    while (!pp_eof(pp)) {
        Token* tok = peek_pp_token(pp);
        if (tok->kind == TokenKind_pp_directive_elif || tok->kind == TokenKind_pp_directive_elifdef ||
            tok->kind == TokenKind_pp_directive_elifndef) {
            // TODO: | and |= is not supported
            // did_include |= preprocess_elif_group(pp, pp->pos, did_include);
            BOOL a = preprocess_elif_group(pp, did_include);
            did_include = did_include ? TRUE : a;
        } else {
            break;
        }
    }
    return did_include;
}

// else-group:
//     '#' 'else' group?
static void preprocess_else_group(Preprocessor* pp, BOOL did_include) {
    skip_pp_token(pp, TokenKind_pp_directive_else);
    skip_whitespaces(pp);
    expect_pp_token(pp, TokenKind_newline);

    include_conditionally(pp, GroupDelimiterKind_after_else_directive, !did_include);
}

// endif-line:
//     '#' 'endif' new-line
static void preprocess_endif_directive(Preprocessor* pp) {
    skip_pp_token(pp, TokenKind_pp_directive_endif);
    skip_whitespaces(pp);
    expect_pp_token(pp, TokenKind_newline);
}

// if-section:
//     if-group elif-groups? else-group? endif-line
static void preprocess_if_section(Preprocessor* pp) {
    BOOL did_include = preprocess_if_group(pp);
    did_include = preprocess_elif_groups_opt(pp, did_include);
    if (peek_pp_token(pp)->kind == TokenKind_pp_directive_else) {
        preprocess_else_group(pp, did_include);
    }
    preprocess_endif_directive(pp);
}

static void preprocess_include_directive(Preprocessor* pp) {
    skip_pp_token(pp, TokenKind_pp_directive_include);
    skip_whitespaces(pp);
    Token* include_name = read_include_header_name(pp);
    const char* include_name_resolved = resolve_include_name(pp, include_name);
    if (include_name_resolved == NULL) {
        fatal_error("%s:%d: cannot resolve include file name: %s", include_name->loc.filename, include_name->loc.line,
                    token_stringify(include_name));
    }

    if (include_name->value.string[0] == '"') {
        BOOL already_included = FALSE;
        for (size_t i = 0; i < pp->included_files->len; ++i) {
            if (strcmp(pp->included_files->data[i], include_name_resolved) == 0) {
                already_included = TRUE;
                break;
            }
        }
        if (!already_included) {
            strings_push(pp->included_files, include_name_resolved);
        }
    }

    skip_whitespaces(pp);
    expect_pp_token(pp, TokenKind_newline);
    expand_include_directive(pp, include_name_resolved, include_name);
}

static void preprocess_embed_directive(Preprocessor* pp) {
    unimplemented();
}

static void preprocess_define_directive(Preprocessor* pp) {
    skip_pp_token(pp, TokenKind_pp_directive_define);
    skip_whitespaces(pp);
    Token* macro_name = expect_pp_token(pp, TokenKind_ident);

    if (consume_pp_token_if(pp, TokenKind_paren_l)) {
        TokenArray* parameters = pp_parse_macro_parameters(pp);
        skip_whitespaces(pp);
        int replacements_start_pos = pp->pos;
        seek_to_next_newline(pp);
        Macro* macro = macros_push_new(pp->macros);
        macro->kind = MacroKind_func;
        macro->name = macro_name->value.string;
        macro->parameters = *parameters;
        int n_replacements = pp->pos - replacements_start_pos;
        tokens_init(&macro->replacements, n_replacements);
        for (int i = 0; i < n_replacements; ++i) {
            *tokens_push_new(&macro->replacements) = *pp_token_at(pp, replacements_start_pos + i);
        }
        // Remove trailing whitespaces.
        for (int i = n_replacements - 1; i >= 0; --i) {
            if (macro->replacements.data[i].kind == TokenKind_whitespace) {
                tokens_pop(&macro->replacements);
            } else {
                break;
            }
        }
    } else {
        skip_whitespaces(pp);
        int replacements_start_pos = pp->pos;
        seek_to_next_newline(pp);
        Macro* macro = macros_push_new(pp->macros);
        macro->kind = MacroKind_obj;
        macro->name = macro_name->value.string;
        int n_replacements = pp->pos - replacements_start_pos;
        tokens_init(&macro->replacements, n_replacements);
        for (int i = 0; i < n_replacements; ++i) {
            *tokens_push_new(&macro->replacements) = *pp_token_at(pp, replacements_start_pos + i);
        }
        // Remove trailing whitespaces.
        for (int i = n_replacements - 1; i >= 0; --i) {
            if (macro->replacements.data[i].kind == TokenKind_whitespace) {
                tokens_pop(&macro->replacements);
            } else {
                break;
            }
        }
    }
    expect_pp_token(pp, TokenKind_newline);
}

static void preprocess_undef_directive(Preprocessor* pp) {
    skip_pp_token(pp, TokenKind_pp_directive_undef);
    skip_whitespaces(pp);
    Token* macro_name = consume_pp_token_if(pp, TokenKind_ident);
    if (macro_name) {
        int macro_idx = find_macro(pp, macro_name->value.string);
        if (macro_idx != -1) {
            undef_macro(pp, macro_idx);
        }
    }
}

static void preprocess_line_directive(Preprocessor* pp) {
    unimplemented();
}

// control-line:
//     ...
//     '#' 'error' pp-tokens? new-line
static void preprocess_error_directive(Preprocessor* pp) {
    // The C23 standard does not specify format of diagnostic message caused by #error.
    // Ducc assumes that #error takes exactly one argument consisting of a string literal.
    // TODO: output some general message or something else if not.
    skip_pp_token(pp, TokenKind_pp_directive_error);
    skip_whitespaces(pp);
    Token* msg = expect_pp_token(pp, TokenKind_literal_str);
    skip_whitespaces(pp);
    expect_pp_token(pp, TokenKind_newline);
    fatal_error("%s:%d: %s", msg->loc.filename, msg->loc.line, msg->value.string);
}

// control-line:
//     ...
//     '#' 'warning' pp-tokens? new-line
static void preprocess_warning_directive(Preprocessor* pp) {
    // The C23 standard does not specify format of diagnostic message caused by #warning.
    // Ducc assumes that #warning takes exactly one argument consisting of a string literal.
    // TODO: output some general message or something else if not.
    skip_pp_token(pp, TokenKind_pp_directive_warning);
    skip_whitespaces(pp);
    Token* msg = expect_pp_token(pp, TokenKind_literal_str);
    skip_whitespaces(pp);
    expect_pp_token(pp, TokenKind_newline);
    fprintf(stderr, "%s:%d: %s", msg->loc.filename, msg->loc.line, msg->value.string);
}

static void preprocess_pragma_directive(Preprocessor* pp) {
    unimplemented();
}

static void preprocess_nop_directive(Preprocessor* pp) {
    skip_pp_token(pp, TokenKind_pp_directive_nop);
}

static void preprocess_non_directive_directive(Preprocessor* pp) {
    Token* tok = peek_pp_token(pp);
    // C23 6.10.1.13:
    // The execution of a non-directive preprocessing directive results in undefined behavior.
    fatal_error("%s:%d: invalid preprocessing directive, '%s'", tok->loc.filename, tok->loc.line, token_stringify(tok));
}

static void preprocess_text_line(Preprocessor* pp) {
    while (!pp_eof(pp)) {
        if (consume_pp_token_if(pp, TokenKind_newline)) {
            return;
        }
        if (consume_pp_token_if_not(pp, TokenKind_ident)) {
            continue;
        }

        BOOL expanded = expand_macro(pp, TRUE);
        if (expanded) {
            // A macro may expand to another macro. Re-scan the expanded tokens.
            // TODO: if the macro is defined recursively, it causes infinite loop.
        } else {
            next_pp_token(pp);
        }
    }
    expect_pp_token(pp, TokenKind_newline);
}

// group-part:
//     if-section
//     control-line
//     '#' non-directive
//     text-line
//
// control-line:
//     '#' 'include' ...
//     '#' 'embed' ...
//     '#' 'define' ...
//     '#' 'undef' ...
//     '#' 'line' ...
//     '#' 'error' ...
//     '#' 'warning' ...
//     '#' 'pragma' ...
//     '#' new-line
static void preprocess_group_part(Preprocessor* pp) {
    Token* tok = peek_pp_token(pp);
    if (tok->kind == TokenKind_pp_directive_if || tok->kind == TokenKind_pp_directive_ifdef ||
        tok->kind == TokenKind_pp_directive_ifndef) {
        preprocess_if_section(pp);
    } else if (tok->kind == TokenKind_pp_directive_elif || tok->kind == TokenKind_pp_directive_elifdef ||
               tok->kind == TokenKind_pp_directive_elifndef || tok->kind == TokenKind_pp_directive_else ||
               tok->kind == TokenKind_pp_directive_endif) {
        fatal_error("%s:%d: unexpected '%s'; no corresponding '#if'*", tok->loc.filename, tok->loc.line,
                    token_kind_stringify(tok->kind));
    } else if (tok->kind == TokenKind_pp_directive_include) {
        preprocess_include_directive(pp);
    } else if (tok->kind == TokenKind_pp_directive_embed) {
        preprocess_embed_directive(pp);
    } else if (tok->kind == TokenKind_pp_directive_define) {
        preprocess_define_directive(pp);
    } else if (tok->kind == TokenKind_pp_directive_undef) {
        preprocess_undef_directive(pp);
    } else if (tok->kind == TokenKind_pp_directive_line) {
        preprocess_line_directive(pp);
    } else if (tok->kind == TokenKind_pp_directive_error) {
        preprocess_error_directive(pp);
    } else if (tok->kind == TokenKind_pp_directive_warning) {
        preprocess_warning_directive(pp);
    } else if (tok->kind == TokenKind_pp_directive_pragma) {
        preprocess_pragma_directive(pp);
    } else if (tok->kind == TokenKind_pp_directive_nop) {
        preprocess_nop_directive(pp);
    } else if (tok->kind == TokenKind_pp_directive_non_directive) {
        preprocess_non_directive_directive(pp);
    } else {
        preprocess_text_line(pp);
    }
}

// group:
//     { group-part }+
static void preprocess_group_opt(Preprocessor* pp, GroupDelimiterKind delimiter_kind) {
    while (!pp_eof(pp)) {
        Token* tok = peek_pp_token(pp);
        if (is_delimiter_of_current_group(delimiter_kind, tok->kind))
            return;
        preprocess_group_part(pp);
    }

    if (delimiter_kind != GroupDelimiterKind_normal) {
        expect_pp_token(pp, TokenKind_pp_directive_endif);
    }
}

static void skip_group_opt(Preprocessor* pp, GroupDelimiterKind delimiter_kind) {
    assert(delimiter_kind != GroupDelimiterKind_normal);
    int nesting = 0;

    while (!pp_eof(pp)) {
        Token* tok = peek_pp_token(pp);
        if (nesting == 0 && is_delimiter_of_current_group(delimiter_kind, tok->kind)) {
            return;
        }
        if (tok->kind == TokenKind_pp_directive_if || tok->kind == TokenKind_pp_directive_ifdef ||
            tok->kind == TokenKind_pp_directive_ifndef) {
            ++nesting;
        } else if (tok->kind == TokenKind_pp_directive_endif) {
            --nesting;
        }
        int first_pos = pp->pos;
        seek_to_next_newline(pp);
        expect_pp_token(pp, TokenKind_newline);
        make_tokens_whitespaces(pp, first_pos, pp->pos);
    }

    expect_pp_token(pp, TokenKind_pp_directive_endif);
}

static void include_conditionally(Preprocessor* pp, GroupDelimiterKind delimiter_kind, BOOL do_include) {
    if (do_include) {
        preprocess_group_opt(pp, delimiter_kind);
    } else {
        skip_group_opt(pp, delimiter_kind);
    }
}

// preprocessing-file:
//     group?
static void preprocess_preprocessing_file(Preprocessor* pp) {
    preprocess_group_opt(pp, GroupDelimiterKind_normal);
}

static void remove_pp_directive(Preprocessor* pp, int directive_token_pos) {
    seek_to_next_newline(pp);
    skip_pp_token(pp, TokenKind_newline);
    make_tokens_whitespaces(pp, directive_token_pos, pp->pos);
}

static void remove_pp_directives(Preprocessor* pp) {
    pp->pos = 0;
    while (!pp_eof(pp)) {
        if (is_pp_directive(peek_pp_token(pp)->kind)) {
            remove_pp_directive(pp, pp->pos);
        } else {
            next_pp_token(pp);
        }
    }
}

static char* get_ducc_include_path() {
    const char* self_dir = get_self_dir();
    char* buf = calloc(strlen(self_dir) + strlen("/../include") + 1, sizeof(char));
    sprintf(buf, "%s/../include", self_dir);
    return buf;
}

static TokenArray* do_preprocess(InFile* src, int depth, MacroArray* macros, StrArray* included_files) {
    TokenArray* pp_tokens = pp_tokenize(src);
    Preprocessor* pp = preprocessor_new(pp_tokens, depth, macros, included_files);
    add_include_path(pp, get_ducc_include_path());
    add_include_path(pp, "/usr/local/include");
    add_include_path(pp, "/usr/include/x86_64-linux-gnu");
    add_include_path(pp, "/usr/include");
    preprocess_preprocessing_file(pp);
    remove_pp_directives(pp);
    return pp->pp_tokens;
}

TokenArray* preprocess(InFile* src, StrArray* included_files) {
    MacroArray* macros = macros_new();
    add_predefined_macros(macros);
    strings_push(included_files, src->loc.filename);
    return do_preprocess(src, 0, macros, included_files);
}
