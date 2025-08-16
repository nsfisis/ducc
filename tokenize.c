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
            int ch = pp_tok->value.string.data[1];
            if (ch == '\\') {
                ch = pp_tok->value.string.data[2];
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
            if (string_equals_cstr(&pp_tok->value.string, "auto")) {
                tok->kind = TokenKind_keyword_auto;
            } else if (string_equals_cstr(&pp_tok->value.string, "break")) {
                tok->kind = TokenKind_keyword_break;
            } else if (string_equals_cstr(&pp_tok->value.string, "case")) {
                tok->kind = TokenKind_keyword_case;
            } else if (string_equals_cstr(&pp_tok->value.string, "char")) {
                tok->kind = TokenKind_keyword_char;
            } else if (string_equals_cstr(&pp_tok->value.string, "const")) {
                tok->kind = TokenKind_keyword_const;
            } else if (string_equals_cstr(&pp_tok->value.string, "continue")) {
                tok->kind = TokenKind_keyword_continue;
            } else if (string_equals_cstr(&pp_tok->value.string, "default")) {
                tok->kind = TokenKind_keyword_default;
            } else if (string_equals_cstr(&pp_tok->value.string, "do")) {
                tok->kind = TokenKind_keyword_do;
            } else if (string_equals_cstr(&pp_tok->value.string, "double")) {
                tok->kind = TokenKind_keyword_double;
            } else if (string_equals_cstr(&pp_tok->value.string, "else")) {
                tok->kind = TokenKind_keyword_else;
            } else if (string_equals_cstr(&pp_tok->value.string, "enum")) {
                tok->kind = TokenKind_keyword_enum;
            } else if (string_equals_cstr(&pp_tok->value.string, "extern")) {
                tok->kind = TokenKind_keyword_extern;
            } else if (string_equals_cstr(&pp_tok->value.string, "float")) {
                tok->kind = TokenKind_keyword_float;
            } else if (string_equals_cstr(&pp_tok->value.string, "for")) {
                tok->kind = TokenKind_keyword_for;
            } else if (string_equals_cstr(&pp_tok->value.string, "goto")) {
                tok->kind = TokenKind_keyword_goto;
            } else if (string_equals_cstr(&pp_tok->value.string, "if")) {
                tok->kind = TokenKind_keyword_if;
            } else if (string_equals_cstr(&pp_tok->value.string, "inline")) {
                tok->kind = TokenKind_keyword_inline;
            } else if (string_equals_cstr(&pp_tok->value.string, "int")) {
                tok->kind = TokenKind_keyword_int;
            } else if (string_equals_cstr(&pp_tok->value.string, "long")) {
                tok->kind = TokenKind_keyword_long;
            } else if (string_equals_cstr(&pp_tok->value.string, "register")) {
                tok->kind = TokenKind_keyword_register;
            } else if (string_equals_cstr(&pp_tok->value.string, "restrict")) {
                tok->kind = TokenKind_keyword_restrict;
            } else if (string_equals_cstr(&pp_tok->value.string, "return")) {
                tok->kind = TokenKind_keyword_return;
            } else if (string_equals_cstr(&pp_tok->value.string, "short")) {
                tok->kind = TokenKind_keyword_short;
            } else if (string_equals_cstr(&pp_tok->value.string, "signed")) {
                tok->kind = TokenKind_keyword_signed;
            } else if (string_equals_cstr(&pp_tok->value.string, "sizeof")) {
                tok->kind = TokenKind_keyword_sizeof;
            } else if (string_equals_cstr(&pp_tok->value.string, "static")) {
                tok->kind = TokenKind_keyword_static;
            } else if (string_equals_cstr(&pp_tok->value.string, "struct")) {
                tok->kind = TokenKind_keyword_struct;
            } else if (string_equals_cstr(&pp_tok->value.string, "switch")) {
                tok->kind = TokenKind_keyword_switch;
            } else if (string_equals_cstr(&pp_tok->value.string, "typedef")) {
                tok->kind = TokenKind_keyword_typedef;
            } else if (string_equals_cstr(&pp_tok->value.string, "union")) {
                tok->kind = TokenKind_keyword_union;
            } else if (string_equals_cstr(&pp_tok->value.string, "unsigned")) {
                tok->kind = TokenKind_keyword_unsigned;
            } else if (string_equals_cstr(&pp_tok->value.string, "void")) {
                tok->kind = TokenKind_keyword_void;
            } else if (string_equals_cstr(&pp_tok->value.string, "volatile")) {
                tok->kind = TokenKind_keyword_volatile;
            } else if (string_equals_cstr(&pp_tok->value.string, "while")) {
                tok->kind = TokenKind_keyword_while;
            } else if (string_equals_cstr(&pp_tok->value.string, "_Bool")) {
                tok->kind = TokenKind_keyword__Bool;
            } else if (string_equals_cstr(&pp_tok->value.string, "_Complex")) {
                tok->kind = TokenKind_keyword__Complex;
            } else if (string_equals_cstr(&pp_tok->value.string, "_Imaginary")) {
                tok->kind = TokenKind_keyword__Imaginary;
            } else if (string_equals_cstr(&pp_tok->value.string, "va_start")) {
                tok->kind = TokenKind_va_start;
                tok->value = pp_tok->value;
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
