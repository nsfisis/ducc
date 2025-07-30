set -e

cat <<'EOF' > expected
main.c
EOF

cat <<'EOF' > header.h
#define A __FILE__
EOF

bash ../../test_diff.sh <<'EOF'
#define B __FILE__
#include "header.h"
int printf();
int main() {
    printf("%s\n", __FILE__);
    // printf("%s\n", B);
    // printf("%s\n", A);
}
EOF
