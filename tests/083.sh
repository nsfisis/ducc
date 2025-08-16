set -e

cat <<'EOF' > expected
main.c:1: unknown preprocessor directive (foo)
EOF

bash ../../test_compile_error.sh <<'EOF'
#foo

int main() {}
EOF

cat <<'EOF' > expected
main.c:1: unknown preprocessor directive (bar)
EOF

bash ../../test_compile_error.sh <<'EOF'
# bar 1 2 3

int main() {}
EOF
