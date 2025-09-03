cat <<'EOF' > expected
123
460
EOF

test_diff <<'EOF'
int printf();

int main() {
    printf("%d\n", 0 | 123);
    printf("%d\n", 12 | 456);
}
EOF

cat <<'EOF' > expected
8
336
EOF
test_diff <<'EOF'
int printf();
int main() {
    printf("%d\n", 1 << 3);
    printf("%d\n", 21 << 4);
}
EOF

cat <<'EOF' > expected
13
0
EOF
test_diff <<'EOF'
int printf();
int main() {
    printf("%d\n", 111 >> 3);
    printf("%d\n", 15 >> 14);
}
EOF

cat <<'EOF' > expected
1
7
6
4
3 2 3 0
EOF
test_diff <<'EOF'
int printf();

int main() {
    int a = 5;
    int b = 3;

    printf("%d\n", a & b);
    printf("%d\n", a | b);
    printf("%d\n", a ^ b);
    printf("%d\n", 2 + 3 & 4);

    int c = 1 + 2 & 3;
    int d = 4 & 5 ^ 6;
    int e = 1 ^ 2 | 3;
    int f = 0 | 1 & 2;
    printf("%d %d %d %d\n", c, d, e, f);
}
EOF

