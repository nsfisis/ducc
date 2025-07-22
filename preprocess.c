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

struct PpDefines {
    PpDefine* data;
    size_t len;
};
typedef struct PpDefines PpDefines;

struct Preprocessor {
    char* src;
    int pos;
    PpToken* pp_tokens;
    int n_pp_tokens;
    PpDefines* pp_defines;
    int include_depth;
    int skip_pp_tokens;
};
typedef struct Preprocessor Preprocessor;

PpToken* do_preprocess(char* src, int depth, PpDefines* pp_defines);

PpDefines* pp_defines_new() {
    PpDefines* pp_defines = calloc(1, sizeof(PpDefines));
    pp_defines->data = calloc(1024, sizeof(PpDefine));
    return pp_defines;
}

void add_predefined_macros(PpDefines* pp_defines) {
    PpDefine* pp_define = pp_defines->data + pp_defines->len;
    pp_define->name.len = strlen("__ducc__");
    pp_define->name.data = "__ducc__";
    pp_define->tokens = calloc(1, sizeof(PpToken));
    pp_define->tokens[0].kind = PpTokenKind_pp_number;
    pp_define->tokens[0].raw.len = strlen("1");
    pp_define->tokens[0].raw.data = "1";
    pp_defines->len += 1;
}

Preprocessor* preprocessor_new(char* src, int include_depth, PpDefines* pp_defines) {
    if (include_depth >= 32) {
        fatal_error("include depth limit exceeded");
    }

    Preprocessor* pp = calloc(1, sizeof(Preprocessor));
    pp->src = src;
    pp->pp_tokens = calloc(1024 * 1024, sizeof(PpToken));
    pp->pp_defines = pp_defines;
    pp->include_depth = include_depth;

    return pp;
}

int find_pp_define(Preprocessor* pp, String* name) {
    int i;
    for (i = 0; i < pp->pp_defines->len; ++i) {
        if (string_equals(&pp->pp_defines->data[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int skip_pp_tokens(Preprocessor* pp) {
    // TODO: support nested #if
    return pp->skip_pp_tokens;
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
                unimplemented();
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
            if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "endif")) {
                ++tok2;
                pp->skip_pp_tokens = 0;
                // Remove #endif directive.
                while (tok != tok2) {
                    tok->kind = PpTokenKind_whitespace;
                    tok->raw.len = 0;
                    tok->raw.data = NULL;
                    ++tok;
                }
            } else if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "else")) {
                ++tok2;
                pp->skip_pp_tokens = 1 - pp->skip_pp_tokens;
                // Remove #else directive.
                while (tok != tok2) {
                    tok->kind = PpTokenKind_whitespace;
                    tok->raw.len = 0;
                    tok->raw.data = NULL;
                    ++tok;
                }
            } else if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "ifdef")) {
                ++tok2;
                while (tok2->kind != PpTokenKind_eof && tok2->kind == PpTokenKind_whitespace)
                    ++tok2;
                if (tok2->kind == PpTokenKind_identifier) {
                    // Process #ifdef directive.
                    PpToken* name = tok2;
                    ++tok2;

                    pp->skip_pp_tokens = find_pp_define(pp, &name->raw) == -1;
                }
                // Remove #ifdef directive.
                while (tok != tok2) {
                    tok->kind = PpTokenKind_whitespace;
                    tok->raw.len = 0;
                    tok->raw.data = NULL;
                    ++tok;
                }
            } else if (skip_pp_tokens(pp)) {
                tok->kind = PpTokenKind_whitespace;
                tok->raw.len = 0;
                tok->raw.data = NULL;
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
                        fatal_error("cannot open include file: %s", include_name_buf);
                    }
                    char* include_source = read_all(include_file);
                    fclose(include_file);

                    PpToken* include_pp_tokens = do_preprocess(include_source, pp->include_depth + 1, pp->pp_defines);

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
            } else if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "define")) {
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

                        PpDefine* pp_define = pp->pp_defines->data + pp->pp_defines->len;
                        pp_define->name.len = define_name->raw.len;
                        pp_define->name.data = define_name->raw.data;
                        pp_define->tokens = calloc(1, sizeof(PpToken));
                        pp_define->tokens[0].kind = define_dest->kind;
                        pp_define->tokens[0].raw.len = define_dest->raw.len;
                        pp_define->tokens[0].raw.data = define_dest->raw.data;
                        ++pp->pp_defines->len;
                    }
                }
                // Remove #define directive.
                while (tok != tok2 + 1) {
                    tok->kind = PpTokenKind_whitespace;
                    tok->raw.len = 0;
                    tok->raw.data = NULL;
                    ++tok;
                }
            }
        } else if (skip_pp_tokens(pp)) {
            tok->kind = PpTokenKind_whitespace;
            tok->raw.len = 0;
            tok->raw.data = NULL;
        } else if (tok->kind == PpTokenKind_identifier) {
            int pp_define_idx = find_pp_define(pp, &tok->raw);
            if (pp_define_idx != -1) {
                define_dest = pp->pp_defines->data[pp_define_idx].tokens;
                tok->kind = define_dest->kind;
                tok->raw.data = define_dest->raw.data;
                tok->raw.len = define_dest->raw.len;
            }
        }
        ++tok;
    }
}

PpToken* do_preprocess(char* src, int depth, PpDefines* pp_defines) {
    Preprocessor* pp = preprocessor_new(src, depth, pp_defines);
    pp_tokenize_all(pp);
    process_pp_directives(pp);
    return pp->pp_tokens;
}

PpToken* preprocess(char* src) {
    PpDefines* pp_defines = pp_defines_new();
    add_predefined_macros(pp_defines);
    return do_preprocess(src, 0, pp_defines);
}
