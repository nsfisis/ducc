cat <<'EOF' > expected
EOF
test_diff <<'EOF'
#define A 1
int main() {
    return 0;
}
EOF

cat <<'EOF' > expected
1,2,3
EOF
test_diff <<'EOF'
int printf();

#define A 1
#define B 2
#define C 3

int main() {
    printf("%d,%d,%d\n", A, B, C);
    return 0;
}
EOF

cat <<'EOF' > expected
0,0,0,0
EOF
test_diff <<'EOF'
int printf();

#define NULL 0
#define TK_EOF        0
#define TY_UNKNOWN 0
#define AST_UNKNOWN       0

int main() {
    printf("%d,%d,%d,%d\n", NULL, TK_EOF, TY_UNKNOWN, AST_UNKNOWN);
    return 0;
}
EOF
