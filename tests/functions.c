#include <helpers.h>

int sprintf(char*, const char*, ...);
int strcmp(const char*, const char*);

int foo() {
    int i;
    int ret;
    i = 0;
    ret = 0;
    for (i = 0; i < 100; i = i + 1) {
        if (i == 12) {
            break;
        }
        ret = ret + i;
    }
    return ret;
}

int f(int a, int b, int c, int d, int e, int f) {
    return a;
}

int f2(int a, int b, int c, int d, int e, int f) {
    return b;
}

int f3(int a, int b, int c, int d, int e, int f) {
    return c;
}

int f4(int a, int b, int c, int d, int e, int f) {
    return d;
}

int f5(int a, int b, int c, int d, int e, int f) {
    return e;
}

int f6(int a, int b, int c, int d, int e, int f) {
    return f;
}

int f7(int select, int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    switch (select) {
    case 0:
        return a;
    case 1:
        return b;
    case 2:
        return c;
    case 3:
        return d;
    case 4:
        return e;
    case 5:
        return f;
    case 6:
        return g;
    case 7:
        return h;
    case 8:
        return i;
    case 9:
        return j;
    }
}

char* f8(char* buf, int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", a, b, c, d, e, f, g, h, i, j);
    return buf;
}

typedef struct {
    long x, y;
} S;

int f9(int select, int a, int b, int c, int d, S e, int f, int g) {
    switch (select) {
    case 0:
        return a;
    case 1:
        return b;
    case 2:
        return c;
    case 3:
        return d;
    case 4:
        return e.x;
    case 5:
        return e.y;
    case 6:
        return f;
    case 7:
        return g;
    }
}

// recursive functions
int fib(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}

int main() {
    // function basics
    ASSERT_EQ(66, foo());
    ASSERT_EQ(10, 10 * f(1, 2, 3, 4, 5, 6));
    ASSERT_EQ(20, 10 * f2(1, 2, 3, 4, 5, 6));
    ASSERT_EQ(30, 10 * f3(1, 2, 3, 4, 5, 6));
    ASSERT_EQ(40, 10 * f4(1, 2, 3, 4, 5, 6));
    ASSERT_EQ(50, 10 * f5(1, 2, 3, 4, 5, 6));
    ASSERT_EQ(60, 10 * f6(1, 2, 3, 4, 5, 6));

    ASSERT_EQ(1, f7(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    ASSERT_EQ(2, f7(1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    ASSERT_EQ(6, f7(5, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    ASSERT_EQ(7, f7(6, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    ASSERT_EQ(8, f7(7, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    ASSERT_EQ(10, f7(9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));

    char buf[100];
    ASSERT_EQ(0, strcmp("1,2,3,4,5,6,7,8,9,10", f8(buf, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)));

    S s;
    s.x = 5;
    s.y = 6;
    ASSERT_EQ(1, f9(0, 1, 2, 3, 4, s, 7, 8));
    ASSERT_EQ(2, f9(1, 1, 2, 3, 4, s, 7, 8));
    ASSERT_EQ(3, f9(2, 1, 2, 3, 4, s, 7, 8));
    ASSERT_EQ(4, f9(3, 1, 2, 3, 4, s, 7, 8));
    ASSERT_EQ(5, f9(4, 1, 2, 3, 4, s, 7, 8));
    ASSERT_EQ(6, f9(5, 1, 2, 3, 4, s, 7, 8));
    ASSERT_EQ(7, f9(6, 1, 2, 3, 4, s, 7, 8));
    ASSERT_EQ(8, f9(7, 1, 2, 3, 4, s, 7, 8));

    // recursive functions
    ASSERT_EQ(89, fib(10));
}
