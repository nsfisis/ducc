#include <helpers.h>
#include <stdarg.h>

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

struct S_var {
    long x;
    long y;
};

enum E {
    A,
    B,
    C,
};

enum E1 {
    E1_A = 10,
    E1_B,
    E1_C = 20,
    E1_D,
};

enum E2 {
    E2_E,
    E2_F = 5,
    E2_G,
    E2_H = E2_G,
};

int main() {
    short a = 42;
    ASSERT_EQ(2, sizeof(a));
    ASSERT_EQ(42, a);
    short* b = &a;
    *b = 123;
    ASSERT_EQ(8, sizeof(b));
    ASSERT_EQ(123, *b);

    ASSERT_EQ(4, sizeof(int));
    ASSERT_EQ(8, sizeof(int*));
    ASSERT_EQ(1, sizeof(char));
    ASSERT_EQ(8, sizeof(char*));
    ASSERT_EQ(8, sizeof(long));
    ASSERT_EQ(8, sizeof(long*));
    ASSERT_EQ(8, sizeof(void*));
    ASSERT_EQ(8, sizeof(int**));
    ASSERT_EQ(8, sizeof(char**));
    ASSERT_EQ(8, sizeof(long**));
    ASSERT_EQ(8, sizeof(void**));
    ASSERT_EQ(8, sizeof(int***));
    ASSERT_EQ(8, sizeof(char***));
    ASSERT_EQ(8, sizeof(long***));
    ASSERT_EQ(8, sizeof(void***));

    ASSERT_EQ(16, sizeof(struct Token));
    ASSERT_EQ(16, sizeof(struct Define));
    ASSERT_EQ(24, sizeof(struct Type));
    ASSERT_EQ(128, sizeof(struct AstNode));
    ASSERT_EQ(16, sizeof(struct LVar));
    ASSERT_EQ(16, sizeof(struct Func));
    ASSERT_EQ(64, sizeof(struct Parser));
    ASSERT_EQ(16, sizeof(struct CodeGen));

    ASSERT_EQ(24, sizeof(va_list));

    int va;
    long vb;
    char vc[123];
    struct S_var vd;
    void* ve;
    struct S_var vf[123];

    ASSERT_EQ(4, sizeof(va));
    ASSERT_EQ(8, sizeof(vb));
    ASSERT_EQ(123, sizeof(vc));
    ASSERT_EQ(16, sizeof(vd));
    ASSERT_EQ(8, sizeof(ve));
    ASSERT_EQ(1968, sizeof(vf));

    ASSERT_EQ(4, sizeof(enum E));
    ASSERT_EQ(0, A);
    ASSERT_EQ(1, B);
    ASSERT_EQ(2, C);

    ASSERT_EQ(10, E1_A);
    ASSERT_EQ(11, E1_B);
    ASSERT_EQ(20, E1_C);
    ASSERT_EQ(21, E1_D);

    ASSERT_EQ(0, E2_E);
    ASSERT_EQ(5, E2_F);
    ASSERT_EQ(6, E2_G);
    ASSERT_EQ(6, E2_H);
}
