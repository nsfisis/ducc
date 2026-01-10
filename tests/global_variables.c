#include <helpers.h>

int printf(const char*, ...);
int strcmp(const char*, const char*);

int a;
int* b = &a;
int c[10];
int* d = c;
int e, *f = e, g[10], *h = g;

char i = 42;
short j = 123;
int k = 999;

char l[6] = "hello";

int main() {
    *b = 123;
    ASSERT_EQ(123, a);

    d[2] = 42;
    ASSERT_EQ(42, c[2]);

    *f = 456;
    ASSERT_EQ(456, e);

    h[5] = 789;
    ASSERT_EQ(789, g[5]);

    ASSERT_EQ(42, i);
    ASSERT_EQ(123, j);
    ASSERT_EQ(999, k);

    ASSERT_EQ(0, strcmp("hello", l));
}
