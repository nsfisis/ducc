#ifndef DUCC_TOKEN_H
#define DUCC_TOKEN_H

#include "io.h"

enum TokenKind {
    TokenKind_eof,

    // Only preprocessing phase.
    TokenKind_hash,
    TokenKind_hashhash,
    TokenKind_whitespace,
    TokenKind_newline,
    TokenKind_other,
    TokenKind_character_constant,
    TokenKind_header_name,
    TokenKind_pp_directive_define,
    TokenKind_pp_directive_elif,
    TokenKind_pp_directive_elifdef,
    TokenKind_pp_directive_elifndef,
    TokenKind_pp_directive_else,
    TokenKind_pp_directive_embed,
    TokenKind_pp_directive_endif,
    TokenKind_pp_directive_error,
    TokenKind_pp_directive_if,
    TokenKind_pp_directive_ifdef,
    TokenKind_pp_directive_ifndef,
    TokenKind_pp_directive_include,
    TokenKind_pp_directive_line,
    TokenKind_pp_directive_non_directive,
    TokenKind_pp_directive_nop,
    TokenKind_pp_directive_pragma,
    TokenKind_pp_directive_undef,
    TokenKind_pp_directive_warning,
    TokenKind_pp_operator_defined,
    TokenKind_pp_operator___has_c_attribute,
    TokenKind_pp_operator___has_embed,
    TokenKind_pp_operator___has_include,

    // C23: 6.4.1
    TokenKind_keyword_alignas,
    TokenKind_keyword_alignof,
    TokenKind_keyword_auto,
    TokenKind_keyword_bool,
    TokenKind_keyword_break,
    TokenKind_keyword_case,
    TokenKind_keyword_char,
    TokenKind_keyword_const,
    TokenKind_keyword_constexpr,
    TokenKind_keyword_continue,
    TokenKind_keyword_default,
    TokenKind_keyword_do,
    TokenKind_keyword_double,
    TokenKind_keyword_else,
    TokenKind_keyword_enum,
    TokenKind_keyword_extern,
    TokenKind_keyword_false,
    TokenKind_keyword_float,
    TokenKind_keyword_for,
    TokenKind_keyword_goto,
    TokenKind_keyword_if,
    TokenKind_keyword_inline,
    TokenKind_keyword_int,
    TokenKind_keyword_long,
    TokenKind_keyword_nullptr,
    TokenKind_keyword_register,
    TokenKind_keyword_restrict,
    TokenKind_keyword_return,
    TokenKind_keyword_short,
    TokenKind_keyword_signed,
    TokenKind_keyword_sizeof,
    TokenKind_keyword_static,
    TokenKind_keyword_static_assert,
    TokenKind_keyword_struct,
    TokenKind_keyword_switch,
    TokenKind_keyword_thread_local,
    TokenKind_keyword_true,
    TokenKind_keyword_typedef,
    TokenKind_keyword_typeof,
    TokenKind_keyword_typeof_unqual,
    TokenKind_keyword_union,
    TokenKind_keyword_unsigned,
    TokenKind_keyword_void,
    TokenKind_keyword_volatile,
    TokenKind_keyword_while,
    TokenKind_keyword__Atomic,
    TokenKind_keyword__BitInt,
    TokenKind_keyword__Complex,
    TokenKind_keyword__Decimal128,
    TokenKind_keyword__Decimal32,
    TokenKind_keyword__Decimal64,
    TokenKind_keyword__Generic,
    TokenKind_keyword__Imaginary,
    TokenKind_keyword__Noreturn,

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
};
typedef enum TokenKind TokenKind;

const char* token_kind_stringify(TokenKind k);

// TokenValue is externally tagged by Token's kind.
union TokenValue {
    const char* string;
    int integer;
};
typedef union TokenValue TokenValue;

struct Token {
    TokenKind kind;
    TokenValue value;
    SourceLocation loc;
};
typedef struct Token Token;

const char* token_stringify(Token* t);

struct TokenArray {
    size_t len;
    size_t capacity;
    Token* data;
};
typedef struct TokenArray TokenArray;

void tokens_init(TokenArray* tokens, size_t capacity);
void tokens_reserve(TokenArray* tokens, size_t size);
Token* tokens_push_new(TokenArray* tokens);
Token* tokens_pop(TokenArray* tokens);

#endif
