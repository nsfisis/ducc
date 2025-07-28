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
            if (pp->src[pp->pos] == '|') {
                ++pp->pos;
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 2;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            } else {
                tok->kind = PpTokenKind_punctuator;
                tok->raw.len = 1;
                tok->raw.data = pp->src + pp->pos - tok->raw.len;
            }
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

PpToken* skip_whitespace(PpToken* tok) {
    while (tok->kind != PpTokenKind_eof && tok->kind == PpTokenKind_whitespace) {
        ++tok;
    }
    return tok;
}

void make_token_whitespace(PpToken* tok) {
    tok->kind = PpTokenKind_whitespace;
    tok->raw.len = 0;
    tok->raw.data = NULL;
}

void remove_directive_tokens(PpToken* start, PpToken* end) {
    PpToken* tok = start;
    while (tok != end) {
        make_token_whitespace(tok);
        ++tok;
    }
}

PpToken* process_endif_directive(Preprocessor* pp, PpToken* tok) {
    PpToken* tok2 = skip_whitespace(tok + 1);
    if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "endif")) {
        ++tok2;
        pp->skip_pp_tokens = 0;
        remove_directive_tokens(tok, tok2);
        return tok2;
    }
    return NULL;
}

PpToken* process_else_directive(Preprocessor* pp, PpToken* tok) {
    PpToken* tok2 = skip_whitespace(tok + 1);
    if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "else")) {
        ++tok2;
        pp->skip_pp_tokens = 1 - pp->skip_pp_tokens;
        remove_directive_tokens(tok, tok2);
        return tok2;
    }
    return NULL;
}

PpToken* process_ifdef_directive(Preprocessor* pp, PpToken* tok) {
    PpToken* tok2 = skip_whitespace(tok + 1);
    if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "ifdef")) {
        ++tok2;
        tok2 = skip_whitespace(tok2);
        if (tok2->kind == PpTokenKind_identifier) {
            PpToken* name = tok2;
            ++tok2;
            pp->skip_pp_tokens = find_pp_define(pp, &name->raw) == -1;
        }
        remove_directive_tokens(tok, tok2);
        return tok2;
    }
    return NULL;
}

PpToken* read_include_header_name(PpToken* tok2, String* include_name) {
    if (tok2->kind == PpTokenKind_string_literal) {
        include_name->data = tok2->raw.data;
        include_name->len = tok2->raw.len;
        ++tok2;
        return tok2;
    } else if (tok2->kind == PpTokenKind_punctuator && string_equals_cstr(&tok2->raw, "<")) {
        char* include_name_start = tok2->raw.data;
        ++tok2;
        int include_name_len = 0;
        while (tok2->kind != PpTokenKind_eof) {
            if (tok2->kind == PpTokenKind_punctuator && string_equals_cstr(&tok2->raw, ">")) {
                break;
            }
            include_name_len += tok2->raw.len;
            ++tok2;
        }
        if (tok2->kind == PpTokenKind_eof) {
            fatal_error("invalid #include: <> not balanced");
        }
        ++tok2;
        include_name->data = include_name_start;
        include_name->len = include_name_len + 2;
        return tok2;
    }
}

const char* resolve_include_name(Preprocessor* pp, String* include_name) {
    char* buf;
    if (include_name->data[0] == '"') {
        buf = calloc(include_name->len - 2 + 1, sizeof(char));
        sprintf(buf, "%.*s", include_name->len - 2, include_name->data + 1);
    } else {
        buf = calloc(include_name->len - 2 + 1 + strlen("/home/ken/src/ducc/include/"), sizeof(char));
        sprintf(buf, "/home/ken/src/ducc/include/%.*s", include_name->len - 2, include_name->data + 1);
    }
    return buf;
}

PpToken* replace_include_directive(Preprocessor* pp, PpToken* tok, PpToken* tok2, const char* include_name_buf) {
    remove_directive_tokens(tok, tok2 + 1);

    FILE* include_file = fopen(include_name_buf, "rb");
    if (include_file == NULL) {
        fatal_error("cannot open include file: %s", include_name_buf);
    }
    char* include_source = read_all(include_file);
    fclose(include_file);

    PpToken* include_pp_tokens = do_preprocess(include_source, pp->include_depth + 1, pp->pp_defines);

    int n_include_pp_tokens = 0;
    while (include_pp_tokens[n_include_pp_tokens].kind != PpTokenKind_eof) {
        ++n_include_pp_tokens;
    }

    int n_pp_tokens_after_include = pp->n_pp_tokens - (tok - pp->pp_tokens);
    memmove(tok + n_include_pp_tokens, tok, n_pp_tokens_after_include * sizeof(PpToken));
    memcpy(tok, include_pp_tokens, n_include_pp_tokens * sizeof(PpToken));
    pp->n_pp_tokens += n_include_pp_tokens;

    return tok + n_include_pp_tokens;
}

PpToken* process_include_directive(Preprocessor* pp, PpToken* tok) {
    PpToken* tok2 = skip_whitespace(tok + 1);

    if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "include")) {
        ++tok2;
        tok2 = skip_whitespace(tok2);
        String* include_name = calloc(1, sizeof(String));
        tok2 = read_include_header_name(tok2, include_name);
        const char* include_name_buf = resolve_include_name(pp, include_name);
        return replace_include_directive(pp, tok, tok2, include_name_buf);
    }
    return NULL;
}

PpToken* process_define_directive(Preprocessor* pp, PpToken* tok) {
    PpToken* tok2 = skip_whitespace(tok + 1);
    if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "define")) {
        ++tok2;
        tok2 = skip_whitespace(tok2);
        if (tok2->kind == PpTokenKind_identifier) {
            PpToken* define_name = tok2;
            ++tok2;
            tok2 = skip_whitespace(tok2);
            if (tok2->kind == PpTokenKind_identifier || tok2->kind == PpTokenKind_pp_number) {
                PpToken* define_dest = tok2;

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
        remove_directive_tokens(tok, tok2 + 1);
        return tok2 + 1;
    }
    return NULL;
}

void expand_macro(Preprocessor* pp, PpToken* tok) {
    int pp_define_idx = find_pp_define(pp, &tok->raw);
    if (pp_define_idx != -1) {
        PpToken* define_dest = pp->pp_defines->data[pp_define_idx].tokens;
        tok->kind = define_dest->kind;
        tok->raw.data = define_dest->raw.data;
        tok->raw.len = define_dest->raw.len;
    }
}

void process_pp_directives(Preprocessor* pp) {
    PpToken* tok = pp->pp_tokens;

    while (tok->kind != PpTokenKind_eof) {
        if (tok->kind == PpTokenKind_punctuator && string_equals_cstr(&tok->raw, "#")) {
            PpToken* next_tok;

            if ((next_tok = process_endif_directive(pp, tok)) != NULL) {
                tok = next_tok;
                continue;
            }
            if ((next_tok = process_else_directive(pp, tok)) != NULL) {
                tok = next_tok;
                continue;
            }
            if ((next_tok = process_ifdef_directive(pp, tok)) != NULL) {
                tok = next_tok;
                continue;
            }
            if (skip_pp_tokens(pp)) {
                make_token_whitespace(tok);
            } else if ((next_tok = process_include_directive(pp, tok)) != NULL) {
                tok = next_tok;
                continue;
            } else if ((next_tok = process_define_directive(pp, tok)) != NULL) {
                tok = next_tok;
                continue;
            }
        } else if (skip_pp_tokens(pp)) {
            make_token_whitespace(tok);
        } else if (tok->kind == PpTokenKind_identifier) {
            expand_macro(pp, tok);
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
