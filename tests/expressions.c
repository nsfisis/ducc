#include <helpers.h>

char get_char() {
    return 65;
}

int glob_x, glob_y, glob_z = 5;

int main() {
    ASSERT_EQ(0, !1);
    ASSERT_EQ(1, !0);
    ASSERT_EQ(0, !23);

    int a = 42;
    ++a;
    a++;
    ASSERT_EQ(44, a);
    ASSERT_EQ(44, a++);
    ASSERT_EQ(46, ++a);
    ASSERT_EQ(46, a);
    --a;
    a--;
    ASSERT_EQ(44, a--);
    ASSERT_EQ(42, --a);
    ASSERT_EQ(42, a);

    int va, vb;
    va = 1, vb = 2;
    int vc = 3, vd = 4;
    ASSERT_EQ(1, va);
    ASSERT_EQ(2, vb);
    ASSERT_EQ(3, vc);
    ASSERT_EQ(4, vd);
    ASSERT_EQ(0, glob_x);
    ASSERT_EQ(0, glob_y);
    ASSERT_EQ(5, glob_z);

    char c = 65;
    int i = (int)c;
    ASSERT_EQ(65, i);

    int i2 = 321;
    char c2 = (char)i2;
    ASSERT_EQ(65, c2);

    short s = 127;
    int i3 = (int)s;
    ASSERT_EQ(127, i3);

    int i4 = 65537;
    short s2 = (short)i4;
    ASSERT_EQ(1, s2);

    long l = 42;
    int i5 = (int)l;
    ASSERT_EQ(42, i5);

    int i6 = 99;
    long l2 = (long)i6;
    ASSERT_EQ(99, (int)l2);

    char c3 = 10;
    short s3 = (short)c3;
    int i7 = (int)s3;
    long l3 = (long)i7;
    ASSERT_EQ(10, (int)l3);

    int ca = 42;
    int cb = -(int)ca;
    ASSERT_EQ(-42, cb);

    char ce = 65;
    int result = (int)ce + (int)ce;
    ASSERT_EQ(130, result);

    char ca2 = 5;
    char cb2 = 5;
    int sum = (int)ca2 + (int)cb2;
    ASSERT_EQ(10, sum);

    short s1 = 10;
    short s2b = 10;
    int sum2 = (int)s1 + (int)s2b;
    ASSERT_EQ(20, sum2);

    long l1 = 15;
    long l2b = 15;
    int sum3 = (int)(l1 + l2b);
    ASSERT_EQ(30, sum3);

    char cn = -10;
    int in = (int)cn;
    ASSERT_EQ(10, -in);

    char cf = get_char();
    int ifr = (int)get_char();
    ASSERT_EQ(65, cf);
    ASSERT_EQ(65, ifr);

    char cmp = 42;
    int icmp = 42;
    ASSERT_EQ(1, (int)cmp == icmp);

    long lc = 55;
    char cc2 = (char)(short)(int)lc;
    ASSERT_EQ(55, cc2);
}
