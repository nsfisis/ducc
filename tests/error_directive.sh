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

cat <<'EOF' > expected
main.c:5: foo
EOF
test_compile_error <<'EOF'
#define \
    A \
    B

#error "foo"
EOF

