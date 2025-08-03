struct Lexer {
    Token* src;
    int pos;
    Token* tokens;
    int n_tokens;
};
typedef struct Lexer Lexer;

Lexer* lexer_new(Token* pp_tokens) {
    Lexer* l = calloc(1, sizeof(Lexer));
    l->src = pp_tokens;
    l->tokens = calloc(1024 * 1024, sizeof(Token));
    return l;
}

void tokenize_all(Lexer* l) {
    int ch;
    int start;
    while (l->src[l->pos].kind != TokenKind_eof) {
        Token* pp_tok = l->src + l->pos;
        Token* tok = l->tokens + l->n_tokens;
        TokenKind k = pp_tok->kind;
        ++l->pos;
        if (k == TokenKind_character_constant) {
            tok->kind = TokenKind_literal_int;
            ch = pp_tok->raw.data[1];
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
        } else if (k == TokenKind_whitespace) {
            continue;
        } else {
            tok->kind = pp_tok->kind;
            tok->raw = pp_tok->raw;
        }
        ++l->n_tokens;
    }
}

Token* tokenize(Token* pp_tokens) {
    Lexer* l = lexer_new(pp_tokens);
    tokenize_all(l);
    return l->tokens;
}
