cat <<'EOF' > expected
400
80
16
EOF

test_diff <<'EOF'
int printf();

int main() {
    int a[10 * 10];
    int b[10 + 10];
    int c[1 << 2];

    printf("%zu\n", sizeof(a));
    printf("%zu\n", sizeof(b));
    printf("%zu\n", sizeof(c));
}
EOF
