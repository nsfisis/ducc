cat <<'EOF' > expected
sizeof(int) = 4
sizeof(int*) = 8
sizeof(char) = 1
sizeof(char*) = 8
sizeof(long) = 8
sizeof(long*) = 8
sizeof(void*) = 8
sizeof(int**) = 8
sizeof(char**) = 8
sizeof(long**) = 8
sizeof(void**) = 8
sizeof(int***) = 8
sizeof(char***) = 8
sizeof(long***) = 8
sizeof(void***) = 8
EOF
test_diff <<'EOF'
int printf();

int main() {
    printf("sizeof(int) = %d\n", sizeof(int));
    printf("sizeof(int*) = %d\n", sizeof(int*));
    printf("sizeof(char) = %d\n", sizeof(char));
    printf("sizeof(char*) = %d\n", sizeof(char*));
    printf("sizeof(long) = %d\n", sizeof(long));
    printf("sizeof(long*) = %d\n", sizeof(long*));
    printf("sizeof(void*) = %d\n", sizeof(void*));
    printf("sizeof(int**) = %d\n", sizeof(int**));
    printf("sizeof(char**) = %d\n", sizeof(char**));
    printf("sizeof(long**) = %d\n", sizeof(long**));
    printf("sizeof(void**) = %d\n", sizeof(void**));
    printf("sizeof(int***) = %d\n", sizeof(int***));
    printf("sizeof(char***) = %d\n", sizeof(char***));
    printf("sizeof(long***) = %d\n", sizeof(long***));
    printf("sizeof(void***) = %d\n", sizeof(void***));
    return 0;
}
EOF

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
test_diff <<'EOF'
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

cat <<'EOF' > expected
24
EOF

test_diff <<'EOF'
#include <stdarg.h>

int printf();

int main() {
    printf("%d\n", sizeof(va_list));
}
EOF

cat <<'EOF' > expected
sizeof(a) = 4
sizeof(b) = 8
sizeof(c) = 123
sizeof(d) = 16
sizeof(e) = 8
sizeof(f) = 1968
EOF
test_diff <<'EOF'
int printf();

struct S {
    long x;
    long y;
};

int main() {
    int a;
    long b;
    char c[123];
    struct S d;
    void* e;
    struct S f[123];

    printf("sizeof(a) = %d\n", sizeof(a));
    printf("sizeof(b) = %d\n", sizeof(b));
    printf("sizeof(c) = %d\n", sizeof(c));
    printf("sizeof(d) = %d\n", sizeof(d));
    printf("sizeof(e) = %d\n", sizeof(e));
    printf("sizeof(f) = %d\n", sizeof(f));
    return 0;
}
EOF
