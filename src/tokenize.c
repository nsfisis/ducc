#include "tokenize.h"
#include "common.h"

struct Lexer {
    TokenArray* src;
    TokenArray* tokens;
};
typedef struct Lexer Lexer;

Lexer* lexer_new(TokenArray* pp_tokens) {
    Lexer* l = calloc(1, sizeof(Lexer));
    l->src = pp_tokens;
    l->tokens = calloc(1, sizeof(TokenArray));
    // l->tokens need not store whitespace tokens.
    tokens_init(l->tokens, pp_tokens->len / 2);
    return l;
}

void tokenize_all(Lexer* l) {
    for (int pos = 0; pos < l->src->len; ++pos) {
        Token* pp_tok = &l->src->data[pos];
        TokenKind k = pp_tok->kind;
        if (k == TokenKind_whitespace || k == TokenKind_newline) {
            continue;
        }
        Token* tok = tokens_push_new(l->tokens);
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
}

TokenArray* tokenize(TokenArray* pp_tokens) {
    Lexer* l = lexer_new(pp_tokens);
    tokenize_all(l);
    return l->tokens;
}
