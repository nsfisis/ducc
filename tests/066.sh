cat <<'EOF' > expected
24
EOF

test_diff <<'EOF'
#include <stdarg.h>

int printf();

int main() {
    printf("%d\n", sizeof(va_list));
}
EOF
