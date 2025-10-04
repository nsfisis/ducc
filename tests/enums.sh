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

cat <<'EOF' > expected
10,11,20,21
0,5,6,6
EOF

test_diff <<'EOF'
int printf();

enum E1 {
    A = 10,
    B,
    C = 20,
    D,
};

enum E2 {
    E,
    F = 5,
    G,
    H = G,
};

int main() {
    printf("%d,%d,%d,%d\n", A, B, C, D);
    printf("%d,%d,%d,%d\n", E, F, G, H);
}
EOF
