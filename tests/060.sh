cat <<'EOF' > expected
42
EOF

cat <<'EOF' > header.h
#define A 42
EOF

test_diff <<'EOF'
#include "header.h"

int printf();

int main() {
    printf("%d\n", A);
    return 0;
}
EOF
