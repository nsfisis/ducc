set -e

cat <<'EOF' > expected
main.c:1: unknown preprocessor directive (foo (<identifier>))
EOF

bash ../../test_compile_error.sh <<'EOF'
#foo

int main() {}
EOF
