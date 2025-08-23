cat <<'EOF' > expected
main.c
main.c
main.c
EOF

cat <<'EOF' > header.h
#define A __FILE__
EOF

test_diff <<'EOF'
#define B __FILE__
#include "header.h"
int printf();
int main() {
    printf("%s\n", __FILE__);
    printf("%s\n", B);
    printf("%s\n", A);
}
EOF
