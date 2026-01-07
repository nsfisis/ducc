#include <helpers.h>

int main() {
    ASSERT_EQ(42, 42);
    ASSERT_EQ(21, 5 + 20 - 4);
    ASSERT_EQ(26, 2 * 3 + 4 * 5);
    ASSERT_EQ(197, (((3 + 5) / 2) + (5 * (9 - 6)) * (5 + 6 * 7)) % 256);
    ASSERT_EQ(30, (-10 + 20 * -3) + 100);
}
