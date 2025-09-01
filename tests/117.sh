cat <<'EOF' > expected
8
4
4
EOF

test_diff <<'EOF'
int printf();

typedef struct {
    int x;
    int y;
} S;

typedef union {
    int a;
    char b;
} U;

typedef enum {
    RED,
    GREEN,
    BLUE
} E;

int main() {
    printf("%zu\n", sizeof(S));
    printf("%zu\n", sizeof(U));
    printf("%zu\n", sizeof(E));
}
EOF
