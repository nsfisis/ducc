cat <<'EOF' > expected
main.c:2: undefined function: f
EOF
test_compile_error <<'EOF'
int main() {
    f();
}
EOF

# TODO: improve error message
# cat <<'EOF' > expected
# main.c:1: expected ';' or '{', but got '}'
# EOF
cat <<'EOF' > expected
main.c:1: expected ';', but got '}'
EOF

test_compile_error <<'EOF'
int main() }
EOF

# TODO: improve error message
# cat <<'EOF' > expected
# main.c:1: expected ';' or '{', but got '}'
# EOF
cat <<'EOF' > expected
main.c:1: expected ';', but got '123'
EOF

test_compile_error <<'EOF'
int main() 123
EOF
