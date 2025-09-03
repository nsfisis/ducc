cat <<'EOF' > expected
42 123 999
EOF

test_diff <<'EOF'
int printf();

char a = 42;
short b = 123;
int c = 999;

int main() {
    printf("%d %d %d\n", a, b, c);
}
EOF

