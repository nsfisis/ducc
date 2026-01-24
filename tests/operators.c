#include <helpers.h>

int main() {
    // arithmetic operators
    ASSERT_EQ(42, 42);
    ASSERT_EQ(21, 5 + 20 - 4);
    ASSERT_EQ(26, 2 * 3 + 4 * 5);
    ASSERT_EQ(197, (((3 + 5) / 2) + (5 * (9 - 6)) * (5 + 6 * 7)) % 256);
    ASSERT_EQ(30, (-10 + 20 * -3) + 100);

    // comparison operators
    ASSERT_EQ(1, 0 == 0);
    ASSERT_EQ(0, 123 != 123);
    ASSERT_EQ(1, 123 != 456);
    ASSERT_EQ(0, 123 == 124);
    ASSERT_EQ(1, 123 < 567);
    ASSERT_EQ(1, 123 <= 567);
    ASSERT_EQ(1, 123 <= 123);
    ASSERT_EQ(0, 123 < 123);

    // bitwise operators
    ASSERT_EQ(123, 0 | 123);
    ASSERT_EQ(460, 12 | 456);

    ASSERT_EQ(8, 1 << 3);
    ASSERT_EQ(336, 21 << 4);
    ASSERT_EQ(13, 111 >> 3);
    ASSERT_EQ(0, 15 >> 14);

    int a = 5;
    int b = 3;
    ASSERT_EQ(1, a & b);
    ASSERT_EQ(7, a | b);
    ASSERT_EQ(6, a ^ b);
    ASSERT_EQ(4, 2 + 3 & 4);

    int c = 1 + 2 & 3;
    int d = 4 & 5 ^ 6;
    int e = 1 ^ 2 | 3;
    int f = 0 | 1 & 2;
    ASSERT_EQ(3, c);
    ASSERT_EQ(2, d);
    ASSERT_EQ(3, e);
    ASSERT_EQ(0, f);

    ASSERT_EQ(-1, ~0);
    ASSERT_EQ(-2, ~1);
    ASSERT_EQ(-6, ~5);
    ASSERT_EQ(0, ~(-1));
    ASSERT_EQ(5, ~(-6));

    int x = 10;
    ASSERT_EQ(-11, ~x);
    ASSERT_EQ(-1, ~(x & 0));
    ASSERT_EQ(-16, ~(x | 5));

    // assignment operators
    int i = 0;
    for (; i < 5; i += 1) {
    }
    ASSERT_EQ(5, i);

    for (i = 5; i >= 0; i -= 1)
        ;
    ASSERT_EQ(-1, i);

    x = 123;
    x *= 456;
    ASSERT_EQ(56088, x);

    int y = 120;
    y /= 5;
    ASSERT_EQ(24, y);

    int z = 17;
    z %= 7;
    ASSERT_EQ(3, z);

    a = 0x05;
    a |= 0x0A;
    ASSERT_EQ(0x0F, a);

    b = 0x0F;
    b &= 0x0A;
    ASSERT_EQ(0x0A, b);

    c = 7;
    c |= 8;
    ASSERT_EQ(15, c);

    d = 15;
    d &= 6;
    ASSERT_EQ(6, d);

    e = 0x0F;
    e ^= 0x05;
    ASSERT_EQ(0x0A, e);

    f = 3;
    f <<= 2;
    ASSERT_EQ(12, f);

    int g = 16;
    g >>= 2;
    ASSERT_EQ(4, g);

    int h = -16;
    h >>= 2;
    ASSERT_EQ(-4, h);

    int j = 1;
    j <<= 4;
    ASSERT_EQ(16, j);

    int k = 64;
    k >>= 3;
    ASSERT_EQ(8, k);

    // ternary operator
    ASSERT_EQ(2, 1 ? 2 : 3);
    ASSERT_EQ(5, 0 ? 4 : 5);

    // sizeof operator
    // sizeof '(' type-name ')'
    ASSERT_EQ(4, sizeof(int));
    ASSERT_EQ(1, sizeof(char));

    // sizeof unary-expr (with parenthesized expressions)
    ASSERT_EQ(4, sizeof(+123));
    ASSERT_EQ(4, sizeof(-1));
    ASSERT_EQ(4, sizeof(1 + 2));
    ASSERT_EQ(4, sizeof(~0));
    ASSERT_EQ(4, sizeof(!0));

    // sizeof unary-expr (variable)
    int sz_var = 42;
    ASSERT_EQ(4, sizeof(sz_var));
    ASSERT_EQ(4, sizeof sz_var);

    // sizeof with more complex expressions
    char sz_c = 'a';
    ASSERT_EQ(1, sizeof sz_c);
    ASSERT_EQ(1, sizeof(sz_c));
    ASSERT_EQ(4, sizeof(sz_c + 1));
}
