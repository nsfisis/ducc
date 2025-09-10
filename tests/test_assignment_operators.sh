test_exit_code 0 <<'EOF'
#include "../../helpers.h"

int main() {
    int i = 0;
    for (; i < 5; i += 1) {}
    ASSERT_EQ(5, i);

    for (i = 5; i >= 0; i -= 1);
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

    return 0;
}
EOF
