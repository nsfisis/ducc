#include "tokenize.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

typedef struct {
    InFile* src;
    bool at_bol;
    bool expect_header_name;
    TokenArray* tokens;
} Lexer;

static Lexer* lexer_new(InFile* src) {
    Lexer* l = calloc(1, sizeof(Lexer));

    l->src = src;
    l->at_bol = true;
    l->expect_header_name = false;
    l->tokens = calloc(1, sizeof(TokenArray));
    tokens_init(l->tokens, 1024 * 16);

    return l;
}

static void pplexer_tokenize_pp_directive(Lexer* l, Token* tok) {
    // Skip whitespaces after '#'.
    char c;
    while (isspace((c = infile_peek_char(l->src)))) {
        if (c == '\n')
            break;
        infile_next_char(l->src);
    }
    // '#' new-line
    if (c == '\n') {
        tok->kind = TokenKind_pp_directive_nop;
        return;
    }

    StrBuilder builder;
    strbuilder_init(&builder);
    while (isalnum(infile_peek_char(l->src))) {
        strbuilder_append_char(&builder, infile_peek_char(l->src));
        infile_next_char(l->src);
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
        l->expect_header_name = true;
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

static void do_tokenize_all(Lexer* l) {
    while (!infile_eof(l->src)) {
        Token* tok = tokens_push_new(l->tokens);
        tok->loc = l->src->loc;
        char c = infile_peek_char(l->src);

        if (l->expect_header_name && c == '"') {
            infile_next_char(l->src);
            StrBuilder builder;
            strbuilder_init(&builder);
            strbuilder_append_char(&builder, '"');
            while (1) {
                char ch = infile_peek_char(l->src);
                if (ch == '"')
                    break;
                strbuilder_append_char(&builder, ch);
                if (ch == '\\') {
                    infile_next_char(l->src);
                    strbuilder_append_char(&builder, infile_peek_char(l->src));
                }
                infile_next_char(l->src);
            }
            strbuilder_append_char(&builder, '"');
            infile_next_char(l->src);
            tok->kind = TokenKind_header_name;
            tok->value.string = builder.buf;
            l->expect_header_name = false;
        } else if (l->expect_header_name && c == '<') {
            infile_next_char(l->src);
            StrBuilder builder;
            strbuilder_init(&builder);
            strbuilder_append_char(&builder, '<');
            while (1) {
                char ch = infile_peek_char(l->src);
                if (ch == '>')
                    break;
                strbuilder_append_char(&builder, ch);
                infile_next_char(l->src);
            }
            strbuilder_append_char(&builder, '>');
            infile_next_char(l->src);
            tok->kind = TokenKind_header_name;
            tok->value.string = builder.buf;
            l->expect_header_name = false;
        } else if (c == '(') {
            infile_next_char(l->src);
            tok->kind = TokenKind_paren_l;
        } else if (c == ')') {
            infile_next_char(l->src);
            tok->kind = TokenKind_paren_r;
        } else if (c == '{') {
            infile_next_char(l->src);
            tok->kind = TokenKind_brace_l;
        } else if (c == '}') {
            infile_next_char(l->src);
            tok->kind = TokenKind_brace_r;
        } else if (c == '[') {
            infile_next_char(l->src);
            tok->kind = TokenKind_bracket_l;
        } else if (c == ']') {
            infile_next_char(l->src);
            tok->kind = TokenKind_bracket_r;
        } else if (c == ',') {
            infile_next_char(l->src);
            tok->kind = TokenKind_comma;
        } else if (c == ':') {
            infile_next_char(l->src);
            tok->kind = TokenKind_colon;
        } else if (c == ';') {
            infile_next_char(l->src);
            tok->kind = TokenKind_semicolon;
        } else if (c == '^') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_assign_xor;
            } else {
                tok->kind = TokenKind_xor;
            }
        } else if (c == '?') {
            infile_next_char(l->src);
            tok->kind = TokenKind_question;
        } else if (c == '~') {
            infile_next_char(l->src);
            tok->kind = TokenKind_tilde;
        } else if (c == '+') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_assign_add;
            } else if (infile_consume_if(l->src, '+')) {
                tok->kind = TokenKind_plusplus;
            } else {
                tok->kind = TokenKind_plus;
            }
        } else if (c == '|') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_assign_or;
            } else if (infile_consume_if(l->src, '|')) {
                tok->kind = TokenKind_oror;
            } else {
                tok->kind = TokenKind_or;
            }
        } else if (c == '&') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_assign_and;
            } else if (infile_consume_if(l->src, '&')) {
                tok->kind = TokenKind_andand;
            } else {
                tok->kind = TokenKind_and;
            }
        } else if (c == '-') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '>')) {
                tok->kind = TokenKind_arrow;
            } else if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_assign_sub;
            } else if (infile_consume_if(l->src, '-')) {
                tok->kind = TokenKind_minusminus;
            } else {
                tok->kind = TokenKind_minus;
            }
        } else if (c == '*') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_assign_mul;
            } else {
                tok->kind = TokenKind_star;
            }
        } else if (c == '/') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_assign_div;
            } else if (infile_consume_if(l->src, '/')) {
                while (!infile_eof(l->src) && infile_peek_char(l->src) != '\n') {
                    infile_next_char(l->src);
                }
                tok->kind = TokenKind_whitespace;
            } else if (infile_consume_if(l->src, '*')) {
                while (infile_peek_char(l->src)) {
                    if (infile_consume_if(l->src, '*')) {
                        if (infile_consume_if(l->src, '/')) {
                            break;
                        }
                        continue;
                    }
                    infile_next_char(l->src);
                }
                tok->kind = TokenKind_whitespace;
            } else {
                tok->kind = TokenKind_slash;
            }
        } else if (c == '%') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_assign_mod;
            } else {
                tok->kind = TokenKind_percent;
            }
        } else if (c == '.') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '.')) {
                if (infile_consume_if(l->src, '.')) {
                    tok->kind = TokenKind_ellipsis;
                } else {
                    tok->kind = TokenKind_other;
                    tok->value.string = "..";
                }
            } else {
                tok->kind = TokenKind_dot;
            }
        } else if (c == '!') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_ne;
            } else {
                tok->kind = TokenKind_not;
            }
        } else if (c == '=') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_eq;
            } else {
                tok->kind = TokenKind_assign;
            }
        } else if (c == '<') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_le;
            } else if (infile_consume_if(l->src, '<')) {
                if (infile_consume_if(l->src, '=')) {
                    tok->kind = TokenKind_assign_lshift;
                } else {
                    tok->kind = TokenKind_lshift;
                }
            } else {
                tok->kind = TokenKind_lt;
            }
        } else if (c == '>') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '=')) {
                tok->kind = TokenKind_ge;
            } else if (infile_consume_if(l->src, '>')) {
                if (infile_consume_if(l->src, '=')) {
                    tok->kind = TokenKind_assign_rshift;
                } else {
                    tok->kind = TokenKind_rshift;
                }
            } else {
                tok->kind = TokenKind_gt;
            }
        } else if (c == '#') {
            infile_next_char(l->src);
            if (infile_consume_if(l->src, '#')) {
                tok->kind = TokenKind_hashhash;
            } else {
                if (l->at_bol) {
                    pplexer_tokenize_pp_directive(l, tok);
                } else {
                    tok->kind = TokenKind_hash;
                }
            }
        } else if (c == '\'') {
            infile_next_char(l->src);
            StrBuilder builder;
            strbuilder_init(&builder);
            strbuilder_append_char(&builder, '\'');
            strbuilder_append_char(&builder, infile_peek_char(l->src));
            if (infile_peek_char(l->src) == '\\') {
                infile_next_char(l->src);
                strbuilder_append_char(&builder, infile_peek_char(l->src));
            }
            strbuilder_append_char(&builder, '\'');
            infile_next_char(l->src);
            infile_next_char(l->src);
            tok->kind = TokenKind_character_constant;
            tok->value.string = builder.buf;
        } else if (c == '"') {
            infile_next_char(l->src);
            StrBuilder builder;
            strbuilder_init(&builder);
            while (1) {
                char ch = infile_peek_char(l->src);
                if (ch == '"')
                    break;
                strbuilder_append_char(&builder, ch);
                if (ch == '\\') {
                    infile_next_char(l->src);
                    strbuilder_append_char(&builder, infile_peek_char(l->src));
                }
                infile_next_char(l->src);
            }
            infile_next_char(l->src);
            tok->kind = TokenKind_literal_str;
            tok->value.string = builder.buf;
        } else if (isdigit(c)) {
            // TODO: implement tokenization of pp-number.
            StrBuilder builder;
            strbuilder_init(&builder);
            while (isalnum(infile_peek_char(l->src))) {
                strbuilder_append_char(&builder, infile_peek_char(l->src));
                infile_next_char(l->src);
            }
            tok->kind = TokenKind_literal_int;
            tok->value.integer = atoi(builder.buf);
        } else if (isalpha(c) || c == '_') {
            StrBuilder builder;
            strbuilder_init(&builder);
            while (isalnum(infile_peek_char(l->src)) || infile_peek_char(l->src) == '_') {
                strbuilder_append_char(&builder, infile_peek_char(l->src));
                infile_next_char(l->src);
            }
            tok->kind = TokenKind_ident;
            tok->value.string = builder.buf;
        } else if (c == '\n') {
            infile_next_char(l->src);
            tok->kind = TokenKind_newline;
        } else if (isspace(c)) {
            while (isspace((c = infile_peek_char(l->src)))) {
                if (c == '\n')
                    break;
                infile_next_char(l->src);
            }
            if (l->at_bol && infile_peek_char(l->src) == '#') {
                infile_next_char(l->src);
                pplexer_tokenize_pp_directive(l, tok);
            } else {
                tok->kind = TokenKind_whitespace;
            }
        } else {
            infile_next_char(l->src);
            tok->kind = TokenKind_other;
            char* buf = calloc(2, sizeof(char));
            buf[0] = c;
            tok->value.string = buf;
        }
        l->at_bol = tok->kind == TokenKind_newline;
    }
    Token* eof_tok = tokens_push_new(l->tokens);
    eof_tok->loc = l->src->loc;
    eof_tok->kind = TokenKind_eof;
}

TokenArray* tokenize(InFile* src) {
    Lexer* l = lexer_new(src);
    do_tokenize_all(l);
    return l->tokens;
}

TokenArray* convert_pp_tokens_to_tokens(TokenArray* pp_tokens) {
    TokenArray* tokens = calloc(1, sizeof(TokenArray));
    // tokens need not store whitespace tokens.
    tokens_init(tokens, pp_tokens->len / 2);

    for (size_t pos = 0; pos < pp_tokens->len; ++pos) {
        Token* pp_tok = &pp_tokens->data[pos];
        TokenKind k = pp_tok->kind;
        if (k == TokenKind_removed || k == TokenKind_whitespace || k == TokenKind_newline) {
            continue;
        }
        Token* tok = tokens_push_new(tokens);
        tok->loc = pp_tok->loc;
        if (k == TokenKind_character_constant) {
            tok->kind = TokenKind_literal_int;
            int ch = pp_tok->value.string[1];
            if (ch == '\\') {
                ch = pp_tok->value.string[2];
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
            tok->value.integer = ch;
        } else if (k == TokenKind_ident) {
            if (strcmp(pp_tok->value.string, "alignas") == 0) {
                tok->kind = TokenKind_keyword_alignas;
            } else if (strcmp(pp_tok->value.string, "alignof") == 0) {
                tok->kind = TokenKind_keyword_alignof;
            } else if (strcmp(pp_tok->value.string, "auto") == 0) {
                tok->kind = TokenKind_keyword_auto;
            } else if (strcmp(pp_tok->value.string, "bool") == 0) {
                tok->kind = TokenKind_keyword_bool;
            } else if (strcmp(pp_tok->value.string, "break") == 0) {
                tok->kind = TokenKind_keyword_break;
            } else if (strcmp(pp_tok->value.string, "case") == 0) {
                tok->kind = TokenKind_keyword_case;
            } else if (strcmp(pp_tok->value.string, "char") == 0) {
                tok->kind = TokenKind_keyword_char;
            } else if (strcmp(pp_tok->value.string, "const") == 0) {
                tok->kind = TokenKind_keyword_const;
            } else if (strcmp(pp_tok->value.string, "constexpr") == 0) {
                tok->kind = TokenKind_keyword_constexpr;
            } else if (strcmp(pp_tok->value.string, "continue") == 0) {
                tok->kind = TokenKind_keyword_continue;
            } else if (strcmp(pp_tok->value.string, "default") == 0) {
                tok->kind = TokenKind_keyword_default;
            } else if (strcmp(pp_tok->value.string, "do") == 0) {
                tok->kind = TokenKind_keyword_do;
            } else if (strcmp(pp_tok->value.string, "double") == 0) {
                tok->kind = TokenKind_keyword_double;
            } else if (strcmp(pp_tok->value.string, "else") == 0) {
                tok->kind = TokenKind_keyword_else;
            } else if (strcmp(pp_tok->value.string, "enum") == 0) {
                tok->kind = TokenKind_keyword_enum;
            } else if (strcmp(pp_tok->value.string, "extern") == 0) {
                tok->kind = TokenKind_keyword_extern;
            } else if (strcmp(pp_tok->value.string, "false") == 0) {
                tok->kind = TokenKind_keyword_false;
            } else if (strcmp(pp_tok->value.string, "float") == 0) {
                tok->kind = TokenKind_keyword_float;
            } else if (strcmp(pp_tok->value.string, "for") == 0) {
                tok->kind = TokenKind_keyword_for;
            } else if (strcmp(pp_tok->value.string, "goto") == 0) {
                tok->kind = TokenKind_keyword_goto;
            } else if (strcmp(pp_tok->value.string, "if") == 0) {
                tok->kind = TokenKind_keyword_if;
            } else if (strcmp(pp_tok->value.string, "inline") == 0) {
                tok->kind = TokenKind_keyword_inline;
            } else if (strcmp(pp_tok->value.string, "int") == 0) {
                tok->kind = TokenKind_keyword_int;
            } else if (strcmp(pp_tok->value.string, "long") == 0) {
                tok->kind = TokenKind_keyword_long;
            } else if (strcmp(pp_tok->value.string, "nullptr") == 0) {
                tok->kind = TokenKind_keyword_nullptr;
            } else if (strcmp(pp_tok->value.string, "register") == 0) {
                tok->kind = TokenKind_keyword_register;
            } else if (strcmp(pp_tok->value.string, "restrict") == 0) {
                tok->kind = TokenKind_keyword_restrict;
            } else if (strcmp(pp_tok->value.string, "return") == 0) {
                tok->kind = TokenKind_keyword_return;
            } else if (strcmp(pp_tok->value.string, "short") == 0) {
                tok->kind = TokenKind_keyword_short;
            } else if (strcmp(pp_tok->value.string, "signed") == 0) {
                tok->kind = TokenKind_keyword_signed;
            } else if (strcmp(pp_tok->value.string, "sizeof") == 0) {
                tok->kind = TokenKind_keyword_sizeof;
            } else if (strcmp(pp_tok->value.string, "static") == 0) {
                tok->kind = TokenKind_keyword_static;
            } else if (strcmp(pp_tok->value.string, "static_assert") == 0) {
                tok->kind = TokenKind_keyword_static_assert;
            } else if (strcmp(pp_tok->value.string, "struct") == 0) {
                tok->kind = TokenKind_keyword_struct;
            } else if (strcmp(pp_tok->value.string, "switch") == 0) {
                tok->kind = TokenKind_keyword_switch;
            } else if (strcmp(pp_tok->value.string, "thread_local") == 0) {
                tok->kind = TokenKind_keyword_thread_local;
            } else if (strcmp(pp_tok->value.string, "true") == 0) {
                tok->kind = TokenKind_keyword_true;
            } else if (strcmp(pp_tok->value.string, "typedef") == 0) {
                tok->kind = TokenKind_keyword_typedef;
            } else if (strcmp(pp_tok->value.string, "typeof") == 0) {
                tok->kind = TokenKind_keyword_typeof;
            } else if (strcmp(pp_tok->value.string, "typeof_unqual") == 0) {
                tok->kind = TokenKind_keyword_typeof_unqual;
            } else if (strcmp(pp_tok->value.string, "union") == 0) {
                tok->kind = TokenKind_keyword_union;
            } else if (strcmp(pp_tok->value.string, "unsigned") == 0) {
                tok->kind = TokenKind_keyword_unsigned;
            } else if (strcmp(pp_tok->value.string, "void") == 0) {
                tok->kind = TokenKind_keyword_void;
            } else if (strcmp(pp_tok->value.string, "volatile") == 0) {
                tok->kind = TokenKind_keyword_volatile;
            } else if (strcmp(pp_tok->value.string, "while") == 0) {
                tok->kind = TokenKind_keyword_while;
            } else if (strcmp(pp_tok->value.string, "_Atomic") == 0) {
                tok->kind = TokenKind_keyword__Atomic;
            } else if (strcmp(pp_tok->value.string, "_BitInt") == 0) {
                tok->kind = TokenKind_keyword__BitInt;
            } else if (strcmp(pp_tok->value.string, "_Complex") == 0) {
                tok->kind = TokenKind_keyword__Complex;
            } else if (strcmp(pp_tok->value.string, "_Decimal128") == 0) {
                tok->kind = TokenKind_keyword__Decimal128;
            } else if (strcmp(pp_tok->value.string, "_Decimal32") == 0) {
                tok->kind = TokenKind_keyword__Decimal32;
            } else if (strcmp(pp_tok->value.string, "_Decimal64") == 0) {
                tok->kind = TokenKind_keyword__Decimal64;
            } else if (strcmp(pp_tok->value.string, "_Generic") == 0) {
                tok->kind = TokenKind_keyword__Generic;
            } else if (strcmp(pp_tok->value.string, "_Imaginary") == 0) {
                tok->kind = TokenKind_keyword__Imaginary;
            } else if (strcmp(pp_tok->value.string, "_Noreturn") == 0) {
                tok->kind = TokenKind_keyword__Noreturn;
            } else {
                tok->kind = TokenKind_ident;
                tok->value = pp_tok->value;
            }
        } else if (k == TokenKind_other) {
            unreachable();
        } else {
            tok->kind = pp_tok->kind;
            tok->value = pp_tok->value;
        }
    }

    return tokens;
}
