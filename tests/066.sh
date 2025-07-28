set -e

cat <<'EOF' > expected
8
EOF

bash ../../test_diff.sh <<'EOF'
#include <stdarg.h>

int printf();

int main() {
    printf("%d\n", sizeof(va_list));
}
EOF
