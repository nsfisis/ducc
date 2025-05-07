set -e

cat <<'EOF' > expected
sizeof(struct Token) = 16
sizeof(struct Define) = 16
sizeof(struct Type) = 24
sizeof(struct AstNode) = 128
sizeof(struct LVar) = 16
sizeof(struct Func) = 16
sizeof(struct Parser) = 64
sizeof(struct CodeGen) = 16
EOF
bash ../../test_diff.sh <<'EOF'
struct Token {
    int kind;
    char* value;
};

struct Define {
    char* from;
    struct Token* to;
};

struct AstNode;

struct Type {
    int kind;
    struct Type* to;
    struct AstNode* members;
};

struct AstNode {
    int kind;
    struct AstNode* next;
    struct AstNode* last;
    char* name;
    struct AstNode* func_params;
    struct AstNode* func_body;
    int int_value;
    struct AstNode* expr1;
    struct AstNode* expr2;
    struct AstNode* expr3;
    int op;
    struct Type* ty;
    int var_index;
    struct AstNode* node1;
    struct AstNode* node2;
    char** str_literals;
};

struct LVar {
    char* name;
    struct Type* ty;
};

struct Func {
    char* name;
    struct Type* ty;
};

struct Parser {
    struct Token* tokens;
    int pos;
    struct LVar* locals;
    int n_locals;
    struct Func* funcs;
    int n_funcs;
    char** str_literals;
    int n_str_literals;
};

struct CodeGen {
    int next_label;
    int* loop_labels;
};

int printf();

int main() {
    printf("sizeof(struct Token) = %d\n", sizeof(struct Token));
    printf("sizeof(struct Define) = %d\n", sizeof(struct Define));
    printf("sizeof(struct Type) = %d\n", sizeof(struct Type));
    printf("sizeof(struct AstNode) = %d\n", sizeof(struct AstNode));
    printf("sizeof(struct LVar) = %d\n", sizeof(struct LVar));
    printf("sizeof(struct Func) = %d\n", sizeof(struct Func));
    printf("sizeof(struct Parser) = %d\n", sizeof(struct Parser));
    printf("sizeof(struct CodeGen) = %d\n", sizeof(struct CodeGen));
    return 0;
}
EOF
