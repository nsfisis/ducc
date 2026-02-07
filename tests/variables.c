#include <helpers.h>
#include <stddef.h>

int printf(const char*, ...);

// global variables
int g_a;
int* g_b = &g_a;
int g_c[10];
int* g_d = g_c;
int g_e, *g_f = g_e, g_g[10], *g_h = g_g;

char g_i = 42;
short g_j = 123;
int g_k = 999;

char g_l[6] = "hello";

char arr1[3] = {65, 66, 67};
short arr2[3] = {10, 20, 30};
int arr3[3] = {1, 2, 3};

struct S1 {
    int x, y;
};
struct S1 arr4[] = {
    {1, 2},
    {3, 4},
};

struct S2 {
    const char* x;
};
struct S2 arr5[] = {
    {"foo"},
    {"bar"},
    {NULL},
};

int main() {
    // global variables
    *g_b = 123;
    ASSERT_EQ(123, g_a);

    g_d[2] = 42;
    ASSERT_EQ(42, g_c[2]);

    *g_f = 456;
    ASSERT_EQ(456, g_e);

    g_h[5] = 789;
    ASSERT_EQ(789, g_g[5]);

    ASSERT_EQ(42, g_i);
    ASSERT_EQ(123, g_j);
    ASSERT_EQ(999, g_k);

    ASSERT_EQ_STR("hello", g_l);

    ASSERT_EQ(65, arr1[0]);
    ASSERT_EQ(66, arr1[1]);
    ASSERT_EQ(67, arr1[2]);

    ASSERT_EQ(10, arr2[0]);
    ASSERT_EQ(20, arr2[1]);
    ASSERT_EQ(30, arr2[2]);

    ASSERT_EQ(1, arr3[0]);
    ASSERT_EQ(2, arr3[1]);
    ASSERT_EQ(3, arr3[2]);

    ASSERT_EQ(1, arr4[0].x);
    ASSERT_EQ(2, arr4[0].y);
    ASSERT_EQ(3, arr4[1].x);
    ASSERT_EQ(4, arr4[1].y);

    ASSERT_EQ_STR("foo", arr5[0].x);
    ASSERT_EQ_STR("bar", arr5[1].x);
    ASSERT_EQ(NULL, arr5[2].x);

    // local variables
    int foo;
    foo = 42;
    ASSERT_EQ(42, foo);

    int bar;
    bar = 28;
    ASSERT_EQ(70, foo + bar);

    int a1;
    int a2;
    int a3;
    int a4;
    int a5;
    int a6;
    int a7;
    int a8;
    int a9;

    a1 = 1;
    a2 = 2;
    a3 = 3;
    a4 = 4;
    a5 = 5;
    a6 = 6;
    a7 = 7;
    a8 = 8;
    a9 = 9;

    ASSERT_EQ(45, a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + 0);

    int d = 2, e = d, f = d + e;
    ASSERT_EQ(2, d);
    ASSERT_EQ(2, e);
    ASSERT_EQ(4, f);

    int arr[3] = {10, 20, 30};
    ASSERT_EQ(10, arr[0]);
    ASSERT_EQ(20, arr[1]);
    ASSERT_EQ(30, arr[2]);
}
