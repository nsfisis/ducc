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
            int ch = pp_tok->raw.data[1];
            if (ch == '\\') {
                ch = pp_tok->raw.data[2];
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
            char* buf = calloc(4, sizeof(char));
            sprintf(buf, "%d", ch);
            tok->raw.data = buf;
            tok->raw.len = strlen(buf);
        } else if (k == TokenKind_literal_str) {
            tok->kind = TokenKind_literal_str;
            tok->raw.data = pp_tok->raw.data + 1;
            tok->raw.len = pp_tok->raw.len - 2;
        } else if (k == TokenKind_other) {
            unreachable();
        } else {
            tok->kind = pp_tok->kind;
            tok->raw = pp_tok->raw;
        }
    }
}

TokenArray* tokenize(TokenArray* pp_tokens) {
    Lexer* l = lexer_new(pp_tokens);
    tokenize_all(l);
    return l->tokens;
}
