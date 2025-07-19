set -e

cat <<'EOF' > expected
12
EOF

cat <<'EOF' > math.h
int multiply(int a, int b) {
    return a * b;
}
EOF

cat <<'EOF' > calc.h
#include "math.h"

int calculate(int x) {
    return multiply(x, 2);
}

int printf(const char*, ...);
EOF

bash ../../test_diff.sh <<'EOF'
#include "calc.h"

int main() {
    printf("%d\n", calculate(6));
    return 0;
}
EOF
