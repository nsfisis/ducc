#include <helpers.h>

void* calloc(long, long);

struct Token {
    int kind;
    char* value;
};

struct Define {
    char* from;
    struct Token* to;
};

struct AstNode0;

struct Type0 {
    int kind;
    struct Type0* to;
    struct AstNode0* members;
};

struct AstNode0 {
    int kind;
    struct AstNode0* next;
    struct AstNode0* last;
    char* name;
    struct AstNode0* func_params;
    struct AstNode0* func_body;
    int int_value;
    struct AstNode0* expr1;
    struct AstNode0* expr2;
    struct AstNode0* expr3;
    int op;
    struct Type0* ty;
    int var_index;
    struct AstNode0* node1;
    struct AstNode0* node2;
    char** str_literals;
};

struct LVar {
    char* name;
    struct Type0* ty;
};

struct Func0 {
    char* name;
    struct Type0* ty;
};

struct Parser {
    struct Token* tokens;
    int pos;
    struct LVar* locals;
    int n_locals;
    struct Func0* funcs;
    int n_funcs;
    char** str_literals;
    int n_str_literals;
};

struct CodeGen {
    int next_label;
    int* loop_labels;
};

struct S1 {
    int a;
    int b;
};

struct T1;

struct S2 {
    struct T1* a;
};

struct T1 {
    int b;
};

struct S3 {
    int a, b;
};

struct T3 {
    short *a, b, c[12];
};

struct S4 {
    int a[5];
};

struct S5 {
    int a;
    int b;
};

struct S6 {
    long a;
    long b;
};
typedef struct S6 S6;

struct S7 {
    int x;
};
typedef struct S7 S7;

struct S8_fwd;
typedef struct S8_fwd S8_fwd;

struct S9 {
    int x;
};

struct S10 {
    int x;
};

struct S11 {
    int x;
};

struct S12 {
    int x;
};

struct S8_fwd {
    int x;
};

typedef struct {
    int x;
    int y;
} AnonS;

typedef union {
    int a;
    char b;
} AnonU;

typedef enum {
    RED,
    GREEN,
    BLUE
} AnonE;

struct S_nested {
    struct {
        int x;
        int y;
    };
    struct {
        int z;
    };
};

int main() {
    struct S1* sp;
    sp = calloc(1, sizeof(struct S1));
    sp->a = 42;
    ASSERT_EQ(42, sp->a);
    (*sp).b = 123;
    ASSERT_EQ(123, (*sp).b);

    struct S2* s2 = calloc(1, sizeof(struct S2));
    s2->a = calloc(1, sizeof(struct T1));
    s2->a->b = 42;
    ASSERT_EQ(42, s2->a->b);

    ASSERT_EQ(8, sizeof(struct S3));
    ASSERT_EQ(40, sizeof(struct T3));

    struct S4 x;
    x.a[0] = 10;
    x.a[1] = 20;
    x.a[2] = 30;
    x.a[3] = 40;
    x.a[4] = 50;
    ASSERT_EQ(20, sizeof(struct S4));
    ASSERT_EQ(10, x.a[0]);
    ASSERT_EQ(20, x.a[1]);
    ASSERT_EQ(30, x.a[2]);
    ASSERT_EQ(40, x.a[3]);
    ASSERT_EQ(50, x.a[4]);

    struct S5* s5 = calloc(1, sizeof(struct S5));
    s5->b = 1;
    ASSERT_EQ(0, s5->a);
    ASSERT_EQ(1, s5->b);

    struct S1* sc1 = calloc(1, sizeof(struct S1));
    struct S1* sc2 = calloc(1, sizeof(struct S1));
    sc1->a = 123;
    sc1->b = 456;
    ASSERT_EQ(123, sc1->a);
    ASSERT_EQ(456, sc1->b);
    ASSERT_EQ(0, sc2->a);
    ASSERT_EQ(0, sc2->b);
    *sc2 = *sc1;
    ASSERT_EQ(123, sc2->a);
    ASSERT_EQ(456, sc2->b);

    S6* sl1 = calloc(1, sizeof(S6));
    S6* sl2 = calloc(1, sizeof(S6));
    sl1->a = 123;
    sl1->b = 456;
    ASSERT_EQ(123, sl1->a);
    ASSERT_EQ(456, sl1->b);
    ASSERT_EQ(0, sl2->a);
    ASSERT_EQ(0, sl2->b);
    *sl2 = *sl1;
    ASSERT_EQ(123, sl2->a);
    ASSERT_EQ(456, sl2->b);

    S6 sv1;
    S6 sv2;
    sv1.a = 123;
    sv1.b = 456;
    sv2.a = 0;
    sv2.b = 0;
    ASSERT_EQ(123, sv1.a);
    ASSERT_EQ(456, sv1.b);
    ASSERT_EQ(0, sv2.a);
    ASSERT_EQ(0, sv2.b);
    sv2 = sv1;
    ASSERT_EQ(123, sv2.a);
    ASSERT_EQ(456, sv2.b);

    S7* s7 = calloc(1, sizeof(S7));
    s7->x = 42;
    ASSERT_EQ(42, s7->x);

    ASSERT_EQ(4, sizeof(S8_fwd));

    ASSERT_EQ(8, sizeof(AnonS));
    ASSERT_EQ(4, sizeof(AnonU));
    ASSERT_EQ(4, sizeof(AnonE));
}
