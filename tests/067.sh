cat <<'EOF' > expected
123
460
EOF

test_diff <<'EOF'
int printf();

int main() {
    printf("%d\n", 0 | 123);
    printf("%d\n", 12 | 456);
}
EOF
