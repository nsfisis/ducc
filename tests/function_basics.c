#include <helpers.h>

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

int main() {
    ASSERT_EQ(66, foo());
    ASSERT_EQ(10, 10 * f(1, 2, 3, 4, 5, 6));
    ASSERT_EQ(20, 10 * f2(1, 2, 3, 4, 5, 6));
    ASSERT_EQ(30, 10 * f3(1, 2, 3, 4, 5, 6));
    ASSERT_EQ(40, 10 * f4(1, 2, 3, 4, 5, 6));
    ASSERT_EQ(50, 10 * f5(1, 2, 3, 4, 5, 6));
    ASSERT_EQ(60, 10 * f6(1, 2, 3, 4, 5, 6));
}
