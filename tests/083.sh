cat <<'EOF' > expected
main.c:1: unknown preprocessor directive (foo)
EOF

test_compile_error <<'EOF'
#foo

int main() {}
EOF

cat <<'EOF' > expected
main.c:1: unknown preprocessor directive (bar)
EOF

test_compile_error <<'EOF'
# bar 1 2 3

int main() {}
EOF
