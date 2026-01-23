#include <helpers.h>

int main() {
    // number literals
    ASSERT_EQ(0, 0);
    ASSERT_EQ(291, 0x123);
    ASSERT_EQ(3405691582, 0xcafebabe);
    ASSERT_EQ(436, 0664);

    // char literals
    ASSERT_EQ(97, 'a');
    ASSERT_EQ(48, '0');
    ASSERT_EQ(92, '\\');
    ASSERT_EQ(39, '\'');
    ASSERT_EQ(10, '\n');

    ASSERT_EQ(39, '\'');
    ASSERT_EQ(34, '\"');
    ASSERT_EQ(63, '\?');
    ASSERT_EQ(92, '\\');
    ASSERT_EQ(7, '\a');
    ASSERT_EQ(8, '\b');
    ASSERT_EQ(12, '\f');
    ASSERT_EQ(10, '\n');
    ASSERT_EQ(13, '\r');
    ASSERT_EQ(9, '\t');
    ASSERT_EQ(11, '\v');

    ASSERT_EQ(0, '\0');

    ASSERT_EQ(27, '\e');

    // bool type
    bool b1 = true, b0 = false;
    ASSERT_EQ(1, b1);
    ASSERT_EQ(0, b0);
    ASSERT_EQ(1, sizeof(b1));
    ASSERT_EQ(1, sizeof(b0));
    ASSERT_EQ(1, sizeof(bool));
}
