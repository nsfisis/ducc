cat <<'EOF' > expected
1 2 3 4
0 0 5
EOF

test_diff <<'EOF'
int printf();
int x, y, z = 5;
int main() {
    int a, b;
    a = 1, b = 2;
    int c = 3, d = 4;
    printf("%d %d %d %d\n", a, b, c, d);
    printf("%d %d %d\n", x, y, z);
}
EOF
