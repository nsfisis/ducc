cat <<'EOF' > expected
2 42
8 123
EOF

test_diff <<'EOF'
int printf();

int main() {
    short a = 42;
    printf("%zu %hd\n", sizeof(a), a);
    short* b = &a;
    *b = 123;
    printf("%zu %hd\n", sizeof(b), *b);
}
EOF
