#include <helpers.h>

union U {
    int i;
    long l;
};

int main() {
    union U u;
    ASSERT_EQ(8, sizeof(u));
    u.l = 42;
    ASSERT_EQ(42, u.i);
    ASSERT_EQ(42, u.l);
}
