cat <<'EOF' > expected
123
EOF

test_diff <<'EOF'
#define foo() 123

int printf();

int main() {
    printf("%d\n", foo());
}
EOF
