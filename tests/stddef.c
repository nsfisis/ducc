#include <helpers.h>
#include <stddef.h>

struct S {
    int x;
    int y;
};
struct T {
    char a;
    int b;
    char c;
};

int main() {
    ASSERT_EQ(0, offsetof(struct S, x));
    ASSERT_EQ(4, offsetof(struct S, y));
    ASSERT_EQ(0, offsetof(struct T, a));
    ASSERT_EQ(4, offsetof(struct T, b));
    ASSERT_EQ(8, offsetof(struct T, c));

    char buf[offsetof(struct T, b)];
    ASSERT_EQ(4, sizeof(buf));
}
