cat <<'EOF' > expected
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

int main() {
    return 0;
}
EOF

cat <<'EOF' > expected
42
123
EOF
test_diff <<'EOF'
struct S {
    int a;
    int b;
};

int printf();
void* calloc();

int main() {
    struct S* sp;
    sp = calloc(1, sizeof(struct S));
    sp->a = 42;
    printf("%d\n", sp->a);
    (*sp).b = 123;
    printf("%d\n", (*sp).b);
    return 0;
}
EOF

cat <<'EOF' > expected
42
EOF
test_diff <<'EOF'
void* calloc();
int printf();

struct T;

struct S {
    struct T* a;
};

struct T {
    int b;
};

int main() {
    struct S* s = calloc(1, sizeof(struct S));
    s->a = calloc(1, sizeof(struct T));
    s->a->b = 42;
    printf("%d\n", s->a->b);
    return 0;
}
EOF

