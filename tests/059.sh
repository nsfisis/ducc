cat <<'EOF' > expected
main.c:1: expected '{', but got '}'
EOF

test_compile_error <<'EOF'
int main() }
EOF

cat <<'EOF' > expected
main.c:1: expected '{', but got '123'
EOF

test_compile_error <<'EOF'
int main() 123
EOF
