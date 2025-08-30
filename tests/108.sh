cat <<'EOF' > expected
123
EOF
test_diff <<'EOF'
#define A(x) x

int printf();

int main() {
    printf("%d\n", A ( 123 ));
}
EOF

cat <<'EOF' > expected
main.c:4: expected ')', but got '<new-line>'
EOF
test_compile_error <<'EOF'
#define A(x) x

int main() {
#if A (
       123) < A(345)
    printf("1\n");
#endif
}
EOF
