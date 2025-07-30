set -e

cat <<'EOF' > expected
5
6 6
7 7
EOF

cat <<'EOF' > header.h
#define A __LINE__
EOF

bash ../../test_diff.sh <<'EOF'
#define B __LINE__
#include "header.h"
int printf();
int main() {
    printf("%d\n", __LINE__);
    printf("%d %d\n", B, B);
    printf("%d %d\n", A, A);
}
EOF
