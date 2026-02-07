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

test_exit_code 0 <<'EOF'
#include <helpers.h>

struct S {
    int a, b;
};

struct T {
    short *a, b, c[12];
};

int main() {
    ASSERT_EQ(8, sizeof(struct S));
    ASSERT_EQ(40, sizeof(struct T));
}
EOF

cat <<'EOF' > expected
20
10
20
30
40
50
EOF

test_diff <<'EOF'
int printf();

struct S {
    int a[5];
};

int main() {
    struct S x;
    x.a[0] = 10;
    x.a[1] = 20;
    x.a[2] = 30;
    x.a[3] = 40;
    x.a[4] = 50;

    printf("%zu\n", sizeof(struct S));

    printf("%d\n", x.a[0]);
    printf("%d\n", x.a[1]);
    printf("%d\n", x.a[2]);
    printf("%d\n", x.a[3]);
    printf("%d\n", x.a[4]);
}
EOF

cat <<'EOF' > expected
0
1
EOF
test_diff <<'EOF'
int printf();
void* calloc();

struct S {
    int a;
    int b;
};

int main() {
    struct S* s = calloc(1, sizeof(struct S));
    s->b = 1;
    printf("%ld\n", s->a);
    printf("%ld\n", s->b);
    return 0;
}
EOF

cat <<'EOF' > expected
123 456
0 0
123 456
EOF

test_diff <<'EOF'
struct S {
    int a;
    int b;
};
typedef struct S S;

void* calloc();
int printf();

int main() {
    S* s1 = calloc(1, sizeof(S));
    S* s2 = calloc(1, sizeof(S));
    s1->a = 123;
    s1->b = 456;
    printf("%d %d\n", s1->a, s1->b);
    printf("%d %d\n", s2->a, s2->b);
    *s2 = *s1;
    printf("%d %d\n", s2->a, s2->b);
}
EOF

cat <<'EOF' > expected
123 456
0 0
123 456
EOF

test_diff <<'EOF'
struct S {
    long a;
    long b;
};
typedef struct S S;

void* calloc();
int printf();

int main() {
    S* s1 = calloc(1, sizeof(S));
    S* s2 = calloc(1, sizeof(S));
    s1->a = 123;
    s1->b = 456;
    printf("%d %d\n", s1->a, s1->b);
    printf("%d %d\n", s2->a, s2->b);
    *s2 = *s1;
    printf("%d %d\n", s2->a, s2->b);
}
EOF

cat <<'EOF' > expected
123 456
0 0
123 456
EOF

test_diff <<'EOF'
struct S {
    long a;
    long b;
};
typedef struct S S;

void* calloc();
int printf();

int main() {
    S s1;
    S s2;
    s1.a = 123;
    s1.b = 456;
    s2.a = 0;
    s2.b = 0;
    printf("%d %d\n", s1.a, s1.b);
    printf("%d %d\n", s2.a, s2.b);
    s2 = s1;
    printf("%d %d\n", s2.a, s2.b);
}
EOF

cat <<'EOF' > expected
42
EOF
test_diff <<'EOF'
void* calloc();
int printf();

struct S {
    int x;
};
typedef struct S S;

int main() {
    S* s = calloc(1, sizeof(S));
    s->x = 42;
    printf("%d\n", s->x);
    return 0;
}
EOF

cat <<'EOF' > expected
4
EOF
test_diff <<'EOF'
struct S0;
typedef struct S0 S0;

struct S1 {
    int x;
};

struct S2 {
    int x;
};

struct S3 {
    int x;
};

struct S4 {
    int x;
};

struct S0 {
    int x;
};

int printf(const char*, ...);
int main() {
    printf("%zu\n", sizeof(S0));
}
EOF

cat <<'EOF' > expected
8
4
4
EOF

test_diff <<'EOF'
int printf();

typedef struct {
    int x;
    int y;
} S;

typedef union {
    int a;
    char b;
} U;

typedef enum {
    RED,
    GREEN,
    BLUE
} E;

int main() {
    printf("%zu\n", sizeof(S));
    printf("%zu\n", sizeof(U));
    printf("%zu\n", sizeof(E));
}
EOF

cat <<'EOF' > expected
EOF

test_diff <<'EOF'
int printf();

struct S1 {
    struct {
        int x;
        int y;
    };
    struct {
        int z;
    };
};

int main() {
    // TODO
    // struct S1 a = {1, 2, 3};
    // printf("%d\n", a.x);
    // printf("%d\n", a.y);
    // printf("%d\n", a.z);
}
EOF
