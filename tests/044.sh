cat <<'EOF' > expected
4
0,1,2
EOF
test_diff <<'EOF'
int printf();

enum E {
    A,
    B,
    C,
};

int main() {
    enum E x = A;
    printf("%d\n", sizeof(enum E));
    printf("%d,%d,%d\n", A, B, C);
    return 0;
}
EOF
