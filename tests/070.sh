set -e

cat <<'EOF' > expected
123
EOF

bash ../../test_diff.sh <<'EOF'
#define foo() 123

int printf();

int main() {
    printf("%d\n", foo());
}
EOF
