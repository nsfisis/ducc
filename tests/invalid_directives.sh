cat <<'EOF' > expected
main.c:1: invalid preprocessing directive, '#foo'
EOF
test_compile_error <<'EOF'
#foo

int main() {}
EOF

cat <<'EOF' > expected
main.c:1: invalid preprocessing directive, '#bar'
EOF
test_compile_error <<'EOF'
# bar 1 2 3

int main() {}
EOF

cat <<'EOF' > expected
EOF
test_diff <<'EOF'
#if 0
#baz
#endif

int main() {}
EOF

