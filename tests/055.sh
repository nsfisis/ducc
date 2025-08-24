cat <<'EOF' > header.h
int add(int a, int b) {
    return a + b;
}

int printf(const char*, ...);
EOF

cat <<'EOF' > expected
8
EOF
test_diff <<'EOF'
#include "header.h"

int main() {
    printf("%d\n", add(5, 3));
    return 0;
}
EOF

mkdir -p foo
cat <<'EOF' > bar.h
#include "baz.h"
EOF
cat <<'EOF' > baz.h
#define A 123
EOF
cat <<'EOF' > foo/bar.h
#include "baz.h"
EOF
cat <<'EOF' > foo/baz.h
#define A 456
EOF

cat <<'EOF' > expected
123
456
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
#include "bar.h"
    printf("%d\n", A);

#undef A

#include "foo/bar.h"
    printf("%d\n", A);
}
EOF
