cat <<'EOF' > expected
main.c:1: foo
EOF
test_compile_error <<'EOF'
#error "foo"
EOF

cat <<'EOF' > expected
EOF
test_diff <<'EOF'
#if 0
#error "foo"
#endif
int main() {}
EOF
