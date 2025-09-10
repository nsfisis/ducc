test_exit_code 0 <<'EOF'
#include "../../helpers.h"

int main() {
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
}
EOF
