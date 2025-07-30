set -e

cat <<'EOF' > expected
5
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
    // printf("%d\n", B);
    // printf("%d\n", A);
}
EOF
