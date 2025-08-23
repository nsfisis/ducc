cat <<'EOF' > expected
579
975
EOF

test_diff <<'EOF'
#define foo 123 + 456
#define bar() 321 + 654

int printf();

int main() {
    printf("%d\n", foo);
    printf("%d\n", bar());
}
EOF
