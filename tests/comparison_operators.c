#include <helpers.h>

int main() {
    ASSERT_EQ(1, 0 == 0);
    ASSERT_EQ(0, 123 != 123);
    ASSERT_EQ(1, 123 != 456);
    ASSERT_EQ(0, 123 == 124);
    ASSERT_EQ(1, 123 < 567);
    ASSERT_EQ(1, 123 <= 567);
    ASSERT_EQ(1, 123 <= 123);
    ASSERT_EQ(0, 123 < 123);
}
