cat <<'EOF' > expected
1
EOF

test_diff <<'EOF'
int printf();

int main() {
    printf("%d\n", __ducc__);
    return 0;
}
EOF

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

