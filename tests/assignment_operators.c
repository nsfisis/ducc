#include <helpers.h>

int main() {
    int i = 0;
    for (; i < 5; i += 1) {
    }
    ASSERT_EQ(5, i);

    for (i = 5; i >= 0; i -= 1)
        ;
    ASSERT_EQ(-1, i);

    int x = 123;
    x *= 456;
    ASSERT_EQ(56088, x);

    int y = 120;
    y /= 5;
    ASSERT_EQ(24, y);

    int z = 17;
    z %= 7;
    ASSERT_EQ(3, z);

    int a = 0x05;
    a |= 0x0A;
    ASSERT_EQ(0x0F, a);

    int b = 0x0F;
    b &= 0x0A;
    ASSERT_EQ(0x0A, b);

    int c = 7;
    c |= 8;
    ASSERT_EQ(15, c);

    int d = 15;
    d &= 6;
    ASSERT_EQ(6, d);

    int e = 0x0F;
    e ^= 0x05;
    ASSERT_EQ(0x0A, e);

    int f = 3;
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
}
