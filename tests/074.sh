cat <<'EOF' > expected
5
6 6
7 7
EOF

cat <<'EOF' > header.h
#define A __LINE__
EOF

test_diff <<'EOF'
#define B __LINE__
#include "header.h"
int printf();
int main() {
    printf("%d\n", __LINE__);
    printf("%d %d\n", B, B);
    printf("%d %d\n", A, A);
}
EOF
