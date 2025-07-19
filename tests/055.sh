set -e

cat <<'EOF' > expected
8
EOF

cat <<'EOF' > header.h
int add(int a, int b) {
    return a + b;
}

int printf(const char*, ...);
EOF

bash ../../test_diff.sh <<'EOF'
#include "header.h"

int main() {
    printf("%d\n", add(5, 3));
    return 0;
}
EOF
