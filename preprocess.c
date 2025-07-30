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

const char* pp_token_kind_stringify(PpTokenKind kind) {
    if (kind == PpTokenKind_eof)
        return "<eof>";
    else if (kind == PpTokenKind_header_name)
        return "<header-name>";
    else if (kind == PpTokenKind_identifier)
        return "<identifier>";
    else if (kind == PpTokenKind_pp_number)
        return "<pp-number>";
    else if (kind == PpTokenKind_character_constant)
        return "<character-constant>";
    else if (kind == PpTokenKind_string_literal)
        return "<string-literal>";
    else if (kind == PpTokenKind_punctuator)
        return "<punctuator>";
    else if (kind == PpTokenKind_other)
        return "<other>";
    else if (kind == PpTokenKind_whitespace)
        return "<whitespace>";
    else
        unreachable();
}

struct PpToken {
    PpTokenKind kind;
    String raw;
};
typedef struct PpToken PpToken;

const char* pp_token_stringify(PpToken* tok) {
    const char* kind_str = pp_token_kind_stringify(tok->kind);
    char* buf = calloc(tok->raw.len + strlen(kind_str) + 3 + 1, sizeof(char));
    sprintf(buf, "%.*s (%s)", tok->raw.len, tok->raw.data, kind_str);
    return buf;
}

enum PpMacroKind {
    PpMacroKind_obj,
    PpMacroKind_func,
};
typedef enum PpMacroKind PpMacroKind;

const char* pp_macro_kind_stringify(PpMacroKind kind) {
    if (kind == PpMacroKind_obj) {
        return "object-like";
    } else if (kind == PpMacroKind_func) {
        return "function-like";
    } else {
        unreachable();
    }
}

struct PpMacro {
    PpMacroKind kind;
    String name;
    size_t n_replacements;
    PpToken* replacements;
};
typedef struct PpMacro PpMacro;

struct PpMacros {
    size_t len;
    PpMacro* data;
};
typedef struct PpMacros PpMacros;

struct Preprocessor {
    char* src;
    int pos;
    PpToken* pp_tokens;
    int n_pp_tokens;
    PpMacros* pp_macros;
    int include_depth;
    int skip_pp_tokens;
    String* include_paths;
    int n_include_paths;
};
typedef struct Preprocessor Preprocessor;

PpToken* do_preprocess(char* src, int depth, PpMacros* pp_macros);

PpMacros* pp_macros_new() {
    PpMacros* pp_macros = calloc(1, sizeof(PpMacros));
    pp_macros->data = calloc(1024, sizeof(PpMacro));
    return pp_macros;
}

void pp_macros_dump(PpMacros* pp_macros) {
    int i;
    fprintf(stderr, "PpMacros {\n");
    fprintf(stderr, "  len = %zu\n", pp_macros->len);
    fprintf(stderr, "  data = [\n");
    for (i = 0; i < pp_macros->len; ++i) {
        PpMacro* m = &pp_macros->data[i];
        fprintf(stderr, "    PpMacro {\n");
        fprintf(stderr, "      kind = %s\n", pp_macro_kind_stringify(m->kind));
        fprintf(stderr, "      name = %.*s\n", m->name.len, m->name.data);
        fprintf(stderr, "      n_replacements = %zu\n", m->n_replacements);
        fprintf(stderr, "      replacements = TODO\n");
        fprintf(stderr, "    }\n");
    }
    fprintf(stderr, "  ]\n");
    fprintf(stderr, "}\n");
}

void add_predefined_macros(PpMacros* pp_macros) {
    PpMacro* pp_macro = pp_macros->data + pp_macros->len;
    pp_macro->kind = PpMacroKind_obj;
    pp_macro->name.len = strlen("__ducc__");
    pp_macro->name.data = "__ducc__";
    pp_macro->n_replacements = 1;
    pp_macro->replacements = calloc(1, sizeof(PpToken));
    pp_macro->replacements[0].kind = PpTokenKind_pp_number;
    pp_macro->replacements[0].raw.len = strlen("1");
    pp_macro->replacements[0].raw.data = "1";
    pp_macros->len += 1;
}

int count_pp_tokens(PpToken* pp_tokens) {
    int n = 0;
    while (pp_tokens[n].kind != PpTokenKind_eof) {
        ++n;
    }
    return n;
}

Preprocessor* preprocessor_new(char* src, int include_depth, PpMacros* pp_macros) {
    if (include_depth >= 32) {
        fatal_error("include depth limit exceeded");
    }

    Preprocessor* pp = calloc(1, sizeof(Preprocessor));
    pp->src = src;
    pp->pp_tokens = calloc(1024 * 1024, sizeof(PpToken));
    pp->pp_macros = pp_macros;
    pp->include_depth = include_depth;
    pp->include_paths = calloc(16, sizeof(String));

    return pp;
}

int find_pp_macro(Preprocessor* pp, String* name) {
    int i;
    for (i = 0; i < pp->pp_macros->len; ++i) {
        if (string_equals(&pp->pp_macros->data[i].name, name)) {
            return i;
        }
    }
    return -1;
}

void add_include_path(Preprocessor* pp, char* include_path) {
    pp->include_paths[pp->n_include_paths].data = include_path;
    pp->include_paths[pp->n_include_paths].len = strlen(include_path);
    ++pp->n_include_paths;
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
            tok->kind = PpTokenKind_whitespace;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
        } else {
            tok->kind = PpTokenKind_other;
            tok->raw.len = 1;
            tok->raw.data = pp->src + pp->pos - tok->raw.len;
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

int string_contains_newline(String* s) {
    int i;
    for (i = 0; i < s->len; ++i) {
        if (s->data[i] == '\n') {
            return 1;
        }
    }
    return 0;
}

PpToken* find_next_newline(PpToken* tok) {
    while (tok->kind != PpTokenKind_eof) {
        if (tok->kind == PpTokenKind_whitespace && string_contains_newline(&tok->raw)) {
            return tok;
        }
        ++tok;
    }
    return NULL;
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
            pp->skip_pp_tokens = find_pp_macro(pp, &name->raw) == -1;
        }
        remove_directive_tokens(tok, tok2);
        return tok2;
    }
    return NULL;
}

PpToken* read_include_header_name(PpToken* tok2, String* include_name) {
    if (tok2->kind == PpTokenKind_string_literal) {
        *include_name = tok2->raw;
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
        return buf;
    } else {
        int i;
        for (i = 0; i < pp->n_include_paths; ++i) {
            buf = calloc(include_name->len - 2 + 1 + pp->include_paths[i].len, sizeof(char));
            sprintf(buf, "%s/%.*s", pp->include_paths[i].data, include_name->len - 2, include_name->data + 1);
            if (access(buf, F_OK | R_OK) == 0) {
                return buf;
            }
        }
        return NULL;
    }
}

PpToken* replace_pp_tokens(Preprocessor* pp, PpToken* dest_start, PpToken* dest_end, int n_source_tokens,
                           PpToken* source_tokens) {
    int n_tokens_to_remove = dest_end - dest_start;
    int n_tokens_after_dest = (pp->pp_tokens + pp->n_pp_tokens) - dest_end;
    int shift_amount;

    if (n_tokens_to_remove < n_source_tokens) {
        // Move existing tokens backward to make room.
        shift_amount = n_source_tokens - n_tokens_to_remove;
        memmove(dest_end + shift_amount, dest_end, n_tokens_after_dest * sizeof(PpToken));
        pp->n_pp_tokens += shift_amount;
    } else if (n_source_tokens < n_tokens_to_remove) {
        // Move existing tokens forward to reduce room.
        shift_amount = n_tokens_to_remove - n_source_tokens;
        memmove(dest_start + n_source_tokens, dest_end, n_tokens_after_dest * sizeof(PpToken));
        pp->n_pp_tokens -= shift_amount;
        memset(pp->pp_tokens + pp->n_pp_tokens, 0, shift_amount * sizeof(PpToken));
    }

    memcpy(dest_start, source_tokens, n_source_tokens * sizeof(PpToken));

    return dest_start + n_source_tokens;
}

PpToken* expand_include_directive(Preprocessor* pp, PpToken* tok, PpToken* tok2, const char* include_name_buf) {
    FILE* include_file = fopen(include_name_buf, "rb");
    if (include_file == NULL) {
        fatal_error("cannot open include file: %s", include_name_buf);
    }
    char* include_source = read_all(include_file);
    fclose(include_file);

    PpToken* include_pp_tokens = do_preprocess(include_source, pp->include_depth + 1, pp->pp_macros);
    return replace_pp_tokens(pp, tok, tok2 + 1, count_pp_tokens(include_pp_tokens), include_pp_tokens);
}

PpToken* process_include_directive(Preprocessor* pp, PpToken* tok) {
    PpToken* tok2 = skip_whitespace(tok + 1);

    if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "include")) {
        ++tok2;
        tok2 = skip_whitespace(tok2);
        String* include_name = calloc(1, sizeof(String));
        tok2 = read_include_header_name(tok2, include_name);
        const char* include_name_buf = resolve_include_name(pp, include_name);
        if (include_name_buf == NULL) {
            fatal_error("cannot resolve include file name: %s", include_name_buf);
        }
        return expand_include_directive(pp, tok, tok2, include_name_buf);
    }
    return NULL;
}

PpToken* process_define_directive(Preprocessor* pp, PpToken* tok) {
    PpToken* tok2 = skip_whitespace(tok + 1);
    PpToken* tok3 = NULL;
    PpMacro* pp_macro;
    int i;
    if (tok2->kind == PpTokenKind_identifier && string_equals_cstr(&tok2->raw, "define")) {
        ++tok2;
        tok2 = skip_whitespace(tok2);
        if (tok2->kind == PpTokenKind_identifier) {
            PpToken* macro_name = tok2;
            ++tok2;
            if (tok2->kind == PpTokenKind_punctuator && string_equals_cstr(&tok2->raw, "(")) {
                ++tok2;
                if (tok2->kind == PpTokenKind_punctuator && string_equals_cstr(&tok2->raw, ")")) {
                    ++tok2;
                } else {
                    fatal_error("#define: invalid function-like macro syntax");
                }
                tok2 = skip_whitespace(tok2);
                tok3 = find_next_newline(tok2);
                if (tok3) {
                    pp_macro = pp->pp_macros->data + pp->pp_macros->len;
                    pp_macro->kind = PpMacroKind_func;
                    pp_macro->name = macro_name->raw;
                    pp_macro->n_replacements = tok3 - tok2;
                    pp_macro->replacements = calloc(pp_macro->n_replacements, sizeof(PpToken));
                    for (i = 0; i < pp_macro->n_replacements; ++i) {
                        pp_macro->replacements[i] = tok2[i];
                    }
                    ++pp->pp_macros->len;
                }
            } else {
                tok2 = skip_whitespace(tok2);
                tok3 = find_next_newline(tok2);
                if (tok3) {
                    pp_macro = pp->pp_macros->data + pp->pp_macros->len;
                    pp_macro->kind = PpMacroKind_obj;
                    pp_macro->name = macro_name->raw;
                    pp_macro->n_replacements = tok3 - tok2;
                    pp_macro->replacements = calloc(pp_macro->n_replacements, sizeof(PpToken));
                    for (i = 0; i < pp_macro->n_replacements; ++i) {
                        pp_macro->replacements[i] = tok2[i];
                    }
                    ++pp->pp_macros->len;
                }
            }
        }
        if (tok3) {
            remove_directive_tokens(tok, tok3);
            return tok3;
        }
    }
    return NULL;
}

void expand_macro(Preprocessor* pp, PpToken* tok) {
    int pp_macro_idx = find_pp_macro(pp, &tok->raw);
    if (pp_macro_idx == -1) {
        return;
    }

    PpMacro* pp_macro = pp->pp_macros->data + pp_macro_idx;
    if (pp_macro->kind == PpMacroKind_func) {
        // also consume '(' and ')'
        replace_pp_tokens(pp, tok, tok + 3, pp_macro->n_replacements, pp_macro->replacements);
    } else {
        replace_pp_tokens(pp, tok, tok + 1, pp_macro->n_replacements, pp_macro->replacements);
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

void pp_dump(PpToken* t, int include_whitespace) {
    for (; t->kind != PpTokenKind_eof; ++t) {
        if (t->kind == PpTokenKind_whitespace && !include_whitespace) {
            continue;
        }
        fprintf(stderr, "%s\n", pp_token_stringify(t));
    }
}

PpToken* do_preprocess(char* src, int depth, PpMacros* pp_macros) {
    Preprocessor* pp = preprocessor_new(src, depth, pp_macros);
    add_include_path(pp, "/home/ken/src/ducc/include");
    add_include_path(pp, "/usr/include");
    pp_tokenize_all(pp);
    process_pp_directives(pp);
    return pp->pp_tokens;
}

PpToken* preprocess(char* src) {
    PpMacros* pp_macros = pp_macros_new();
    add_predefined_macros(pp_macros);
    return do_preprocess(src, 0, pp_macros);
}
