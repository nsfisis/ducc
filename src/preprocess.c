#include "preprocess.h"
#include <libgen.h>
#include <unistd.h>
#include "common.h"
#include "parse.h"
#include "sys.h"
#include "tokenize.h"

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

// Accept "FOO" or "FOO=value"
static void define_macro_from_string(MacroArray* macros, const char* def) {
    Macro* m = macros_push_new(macros);
    m->kind = MacroKind_obj;

    const char* eq = strchr(def, '=');
    if (eq) {
        // FOO=value format
        size_t name_len = eq - def;
        char* name = calloc(name_len + 1, sizeof(char));
        memcpy(name, def, name_len);
        m->name = name;

        const char* value = eq + 1;
        tokens_init(&m->replacements, 1);
        Token* tok = tokens_push_new(&m->replacements);

        // Try to parse as integer
        char* num_end;
        long int_val = strtol(value, &num_end, 10);
        if (value[0] != '\0' && *num_end == '\0') {
            tok->kind = TokenKind_literal_int;
            tok->value.integer = int_val;
        } else {
            tok->kind = TokenKind_ident;
            tok->value.string = value;
        }
    } else {
        // FOO format (equivalent to FOO=1)
        m->name = def;
        tokens_init(&m->replacements, 1);
        Token* tok = tokens_push_new(&m->replacements);
        tok->kind = TokenKind_literal_int;
        tok->value.integer = 1;
    }
}

static void add_user_defines(MacroArray* macros, StrArray* user_defines) {
    for (size_t i = 0; i < user_defines->len; ++i) {
        define_macro_from_string(macros, user_defines->data[i]);
    }
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
    TokenArray* pp_tokens;
    int pos;
    MacroArray* macros;
    int include_depth;
    StrArray include_paths;
    StrArray* included_files;
} Preprocessor;

static TokenArray* do_preprocess(InFile* src, int depth, MacroArray* macros, StrArray* included_files,
                                 StrArray* user_include_dirs);

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

static bool pp_eof(Preprocessor* pp) {
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

static void skip_whitespaces_or_newlines(Preprocessor* pp, bool skip_newline) {
    while (!pp_eof(pp) && (consume_pp_token_if(pp, TokenKind_whitespace) ||
                           (skip_newline && consume_pp_token_if(pp, TokenKind_newline))))
        ;
}

// It will not consume a new-line token.
static void seek_to_next_newline(Preprocessor* pp) {
    while (!pp_eof(pp) && consume_pp_token_if_not(pp, TokenKind_newline))
        ;
}

static void make_tokens_removed(Preprocessor* pp, int start, int end) {
    for (int i = start; i < end; ++i) {
        Token* tok = pp_token_at(pp, i);
        tok->kind = TokenKind_removed;
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
        do_preprocess(include_source, pp->include_depth + 1, pp->macros, pp->included_files, NULL);
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
static MacroArgArray* pp_parse_macro_arguments(Preprocessor* pp, bool skip_newline) {
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
        while (true) {
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

typedef struct MacroExpansionContext {
    // Stack of macro names that have been already expanded.
    StrArray already_expanded;
} MacroExpansionContext;

MacroExpansionContext* macroexpansioncontext_new() {
    MacroExpansionContext* ctx = calloc(1, sizeof(MacroExpansionContext));
    strings_init(&ctx->already_expanded);
    return ctx;
}

static int expand_macro(Preprocessor* pp, bool skip_newline, MacroExpansionContext* ctx);

static void expand_macro_arg(Preprocessor* pp, MacroArg* arg, bool skip_newline, MacroExpansionContext* ctx) {
    tokens_push_new(&arg->tokens)->kind = TokenKind_eof;

    Preprocessor* pp2 = preprocessor_new(&arg->tokens, pp->include_depth, pp->macros, pp->included_files);

    size_t arg_token_count = arg->tokens.len;
    size_t processed_token_count = 0;
    while (processed_token_count < arg_token_count) {
        if (peek_pp_token(pp2)->kind == TokenKind_ident) {
            processed_token_count += expand_macro(pp2, skip_newline, ctx);
        } else {
            next_pp_token(pp2);
            processed_token_count += 1;
        }
    }

    tokens_pop(&arg->tokens);
}

static int expand_macro(Preprocessor* pp, bool skip_newline, MacroExpansionContext* ctx) {
    if (ctx == NULL) {
        ctx = macroexpansioncontext_new();
    }

    int macro_name_pos = pp->pos;
    Token* macro_name = peek_pp_token(pp);
    const char* macro_name_str = macro_name->value.string;

    // Supress expansion if the macro has already been expanded.
    for (size_t i = 0; i < ctx->already_expanded.len; ++i) {
        if (strcmp(ctx->already_expanded.data[i], macro_name->value.string) == 0) {
            next_pp_token(pp);
            return 1;
        }
    }

    int macro_idx = find_macro(pp, macro_name->value.string);
    if (macro_idx == -1) {
        next_pp_token(pp);
        return 1;
    }

    SourceLocation original_loc = macro_name->loc;
    size_t token_count_before_expansion;
    size_t token_count_after_expansion;
    Macro* macro = &pp->macros->data[macro_idx];
    if (macro->kind == MacroKind_func) {
        next_pp_token(pp);
        MacroArgArray* args = pp_parse_macro_arguments(pp, skip_newline);
        token_count_before_expansion = pp->pos - macro_name_pos;
        replace_pp_tokens(pp, macro_name_pos, pp->pos, &macro->replacements);

        bool* no_expand = calloc(macro->parameters.len, sizeof(bool));
        for (size_t i = 0; i < macro->replacements.len; ++i) {
            if (macro->replacements.data[i].kind != TokenKind_hashhash)
                continue;

            Token* lhs = NULL;
            for (int j = i - 1; j >= 0; --j) {
                if (macro->replacements.data[j].kind != TokenKind_whitespace) {
                    lhs = &macro->replacements.data[j];
                    break;
                }
            }
            Token* rhs = NULL;
            for (int j = i + 1; j < macro->replacements.len; ++j) {
                if (macro->replacements.data[j].kind != TokenKind_whitespace) {
                    rhs = &macro->replacements.data[j];
                    break;
                }
            }
            if (lhs) {
                int param1 = macro_find_param(macro, lhs);
                if (param1 != -1) {
                    no_expand[param1] = true;
                }
            }
            if (rhs) {
                int param2 = macro_find_param(macro, rhs);
                if (param2 != -1) {
                    no_expand[param2] = true;
                }
            }
        }

        // Argument expansion
        for (size_t i = 0; i < args->len; ++i) {
            if (no_expand[i])
                continue;
            MacroArg* arg = &args->data[i];
            expand_macro_arg(pp, arg, skip_newline, ctx);
        }

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
                int lhs_pos = -1;
                for (int j = pos - 1; j >= macro_name_pos; --j) {
                    if (pp_token_at(pp, j)->kind != TokenKind_whitespace) {
                        lhs_pos = j;
                        break;
                    }
                }
                int rhs_pos = -1;
                for (size_t j = pos + 1; j < macro_name_pos + token_count; ++j) {
                    if (pp_token_at(pp, j)->kind != TokenKind_whitespace) {
                        rhs_pos = j;
                        break;
                    }
                }
                if (lhs_pos == -1 || rhs_pos == -1) {
                    fatal_error("invalid usage of ## operator");
                }
                Token* concatenated = concat_two_tokens(pp_token_at(pp, lhs_pos), pp_token_at(pp, rhs_pos));

                // Replace the three tokens (lhs ## rhs) with the concatenated one
                TokenArray single_token;
                tokens_init(&single_token, 1);
                *tokens_push_new(&single_token) = *concatenated;
                replace_pp_tokens(pp, lhs_pos, rhs_pos + 1, &single_token);
                token_count -= rhs_pos - lhs_pos;
                i -= pos - lhs_pos;
                token_count2 -= pos - lhs_pos - 1;
            } else {
                ++token_count2;
            }
        }

        // Inherit a source location from the original macro token.
        for (size_t i = 0; i < token_count2; ++i) {
            pp_token_at(pp, macro_name_pos + i)->loc = original_loc;
        }
        token_count_after_expansion = token_count2;
    } else if (macro->kind == MacroKind_obj) {
        replace_pp_tokens(pp, macro_name_pos, macro_name_pos + 1, &macro->replacements);
        // Inherit a source location from the original macro token.
        for (size_t i = 0; i < macro->replacements.len; ++i) {
            pp_token_at(pp, macro_name_pos + i)->loc = original_loc;
        }
        token_count_before_expansion = 1;
        token_count_after_expansion = macro->replacements.len;
    } else if (macro->kind == MacroKind_builtin_file) {
        Token file_tok;
        file_tok.kind = TokenKind_literal_str;
        file_tok.value.string = macro_name->loc.filename;
        file_tok.loc.filename = NULL;
        file_tok.loc.line = 0;
        replace_single_pp_token(pp, macro_name_pos, &file_tok);
        token_count_before_expansion = 1;
        token_count_after_expansion = 1;
    } else if (macro->kind == MacroKind_builtin_line) {
        Token line_tok;
        line_tok.kind = TokenKind_literal_int;
        line_tok.value.integer = macro_name->loc.line;
        line_tok.loc.filename = NULL;
        line_tok.loc.line = 0;
        replace_single_pp_token(pp, macro_name_pos, &line_tok);
        token_count_before_expansion = 1;
        token_count_after_expansion = 1;
    } else {
        unreachable();
    }

    // Recursive expansion.
    strings_push(&ctx->already_expanded, macro_name_str);
    pp->pos = macro_name_pos;
    size_t processed_token_count = 0;
    while (processed_token_count < token_count_after_expansion) {
        if (peek_pp_token(pp)->kind == TokenKind_ident) {
            processed_token_count += expand_macro(pp, skip_newline, ctx);
        } else {
            next_pp_token(pp);
            processed_token_count += 1;
        }
    }
    strings_pop(&ctx->already_expanded);

    return token_count_before_expansion;
}

typedef enum {
    GroupDelimiterKind_normal,
    GroupDelimiterKind_after_if_directive,
    GroupDelimiterKind_after_else_directive,
} GroupDelimiterKind;

static bool is_delimiter_of_current_group(GroupDelimiterKind delimiter_kind, TokenKind token_kind) {
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
static void include_conditionally(Preprocessor* pp, GroupDelimiterKind delimiter_kind, bool do_include);

static bool preprocess_if_group_or_elif_group(Preprocessor* pp, bool did_include) {
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
                    bool is_defined = find_macro(pp, macro_name->value.string) != -1;
                    TokenArray defined_results;
                    tokens_init(&defined_results, 1);
                    Token* defined_result = tokens_push_new(&defined_results);
                    defined_result->kind = TokenKind_literal_int;
                    defined_result->value.integer = is_defined;
                    pp->pos = replace_pp_tokens(pp, defined_pos, pp->pos, &defined_results);
                } else {
                    expand_macro(pp, false, NULL);
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
                bool is_true = strcmp(tok->value.string, "true") == 0;
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

        bool do_include = pp_eval_constant_expression(&condition_expression_tokens) && !did_include;
        include_conditionally(pp, GroupDelimiterKind_after_if_directive, do_include);
        return do_include;
    } else if (directive->kind == TokenKind_pp_directive_ifdef || directive->kind == TokenKind_pp_directive_elifdef) {
        skip_whitespaces(pp);
        Token* macro_name = consume_pp_token_if(pp, TokenKind_ident);
        if (!macro_name) {
            fatal_error("");
        }

        bool do_include = !did_include && find_macro(pp, macro_name->value.string) != -1;
        include_conditionally(pp, GroupDelimiterKind_after_if_directive, do_include);
        return do_include;
    } else if (directive->kind == TokenKind_pp_directive_ifndef || directive->kind == TokenKind_pp_directive_elifndef) {
        skip_whitespaces(pp);
        Token* macro_name = consume_pp_token_if(pp, TokenKind_ident);
        if (!macro_name) {
            fatal_error("");
        }

        bool do_include = !did_include && find_macro(pp, macro_name->value.string) == -1;
        include_conditionally(pp, GroupDelimiterKind_after_if_directive, do_include);
        return do_include;
    } else {
        unreachable();
    }
}

static bool preprocess_if_group(Preprocessor* pp) {
    return preprocess_if_group_or_elif_group(pp, false);
}

static bool preprocess_elif_group(Preprocessor* pp, bool did_include) {
    return preprocess_if_group_or_elif_group(pp, did_include);
}

// elif-groups:
//     { elif-group }+
static bool preprocess_elif_groups_opt(Preprocessor* pp, bool did_include) {
    while (!pp_eof(pp)) {
        Token* tok = peek_pp_token(pp);
        if (tok->kind == TokenKind_pp_directive_elif || tok->kind == TokenKind_pp_directive_elifdef ||
            tok->kind == TokenKind_pp_directive_elifndef) {
            did_include |= preprocess_elif_group(pp, did_include);
        } else {
            break;
        }
    }
    return did_include;
}

// else-group:
//     '#' 'else' group?
static void preprocess_else_group(Preprocessor* pp, bool did_include) {
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
    bool did_include = preprocess_if_group(pp);
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
        bool already_included = false;
        for (size_t i = 0; i < pp->included_files->len; ++i) {
            if (strcmp(pp->included_files->data[i], include_name_resolved) == 0) {
                already_included = true;
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

        expand_macro(pp, true, NULL);
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
        make_tokens_removed(pp, first_pos, pp->pos);
    }

    expect_pp_token(pp, TokenKind_pp_directive_endif);
}

static void include_conditionally(Preprocessor* pp, GroupDelimiterKind delimiter_kind, bool do_include) {
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
    make_tokens_removed(pp, directive_token_pos, pp->pos);
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

static TokenArray* do_preprocess(InFile* src, int depth, MacroArray* macros, StrArray* included_files,
                                 StrArray* user_include_dirs) {
    TokenArray* pp_tokens = tokenize(src);
    Preprocessor* pp = preprocessor_new(pp_tokens, depth, macros, included_files);

    // Ducc's built-in headers has highest priority.
    add_include_path(pp, get_ducc_include_path());

    if (user_include_dirs) {
        for (size_t i = 0; i < user_include_dirs->len; ++i) {
            add_include_path(pp, user_include_dirs->data[i]);
        }
    }
    add_include_path(pp, "/usr/local/include");
    add_include_path(pp, "/usr/include/x86_64-linux-gnu");
    add_include_path(pp, "/usr/include");

    preprocess_preprocessing_file(pp);
    remove_pp_directives(pp);
    return pp->pp_tokens;
}

TokenArray* preprocess(InFile* src, StrArray* included_files, StrArray* user_include_dirs, StrArray* user_defines) {
    MacroArray* macros = macros_new();
    add_predefined_macros(macros);
    add_user_defines(macros, user_defines);
    strings_push(included_files, src->loc.filename);
    return do_preprocess(src, 0, macros, included_files, user_include_dirs);
}

void concat_adjacent_string_literals(TokenArray* pp_tokens) {
    size_t last_nonempty_token_pos = 0;
    TokenKind last_nonempty_token_kind = TokenKind_eof;
    for (size_t pos = 0; pos < pp_tokens->len; ++pos) {
        Token* pp_tok = &pp_tokens->data[pos];
        TokenKind k = pp_tok->kind;
        if (k == TokenKind_removed || k == TokenKind_whitespace || k == TokenKind_newline) {
            continue;
        }
        if (k == TokenKind_literal_str && last_nonempty_token_kind == TokenKind_literal_str) {
            // Concatenate adjacent string literals.
            Token* last_pp_tok = &pp_tokens->data[last_nonempty_token_pos];
            const char* s1 = last_pp_tok->value.string;
            size_t l1 = strlen(s1);
            const char* s2 = pp_tok->value.string;
            size_t l2 = strlen(s2);
            char* buf = calloc(l1 + l2 + 1, sizeof(char));
            memcpy(buf, s1, l1);
            memcpy(buf + l1, s2, l2);
            last_pp_tok->value.string = buf;
            pp_tok->kind = TokenKind_removed;
        } else {
            last_nonempty_token_pos = pos;
            last_nonempty_token_kind = k;
        }
    }
}

void print_token_to_file(FILE* out, TokenArray* pp_tokens) {
    for (size_t i = 0; i < pp_tokens->len; ++i) {
        Token* tok = &pp_tokens->data[i];

        if (tok->kind == TokenKind_whitespace) {
            // TODO: preserve indent?
            fprintf(out, " ");
        } else if (tok->kind == TokenKind_removed) {
            // Output nothing for removed tokens
        } else if (tok->kind == TokenKind_newline) {
            // TODO: remove adjacent newlines?
            fprintf(out, "\n");
        } else if (tok->kind != TokenKind_eof) {
            // TODO: string literal
            fprintf(out, "%s", token_stringify(tok));
            // Add space after token if next token is not punctuation
            // TODO: apply stricter approach
            if (i + 1 < pp_tokens->len) {
                Token* next = &pp_tokens->data[i + 1];
                if (next->kind != TokenKind_newline && next->kind != TokenKind_whitespace &&
                    next->kind != TokenKind_removed && next->kind != TokenKind_eof && next->kind != TokenKind_comma &&
                    next->kind != TokenKind_semicolon && next->kind != TokenKind_paren_r &&
                    next->kind != TokenKind_bracket_r && next->kind != TokenKind_brace_r &&
                    next->kind != TokenKind_dot) {
                    fprintf(out, " ");
                }
            }
        }
    }
}
