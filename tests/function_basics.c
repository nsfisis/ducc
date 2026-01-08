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

int main() {
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
}
