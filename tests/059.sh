set -e

cat <<'EOF' > expected
main.c:1: expected '{', but got '}'
EOF

bash ../../test_compile_error.sh <<'EOF'
int main() }
EOF

cat <<'EOF' > expected
main.c:1: expected '{', but got '123 (<integer>)'
EOF

bash ../../test_compile_error.sh <<'EOF'
int main() 123
EOF
