cat <<'EOF' > expected
42 123 999
EOF

test_diff <<'EOF'
int printf();

char a = 42;
short b = 123;
int c = 999;

int main() {
    printf("%d %d %d\n", a, b, c);
}
EOF

test_exit_code 0 <<'EOF'
#include "../../helpers.h"

int a;
int* b = &a;
int c[10];
int* d = c;
int e, *f = e, g[10], *h = g;

int main() {
    *b = 123;
    ASSERT_EQ(123, a);

    d[2] = 42;
    ASSERT_EQ(42, c[2]);

    *f = 456;
    ASSERT_EQ(456, e);

    h[5] = 789;
    ASSERT_EQ(789, g[5]);
}
EOF
