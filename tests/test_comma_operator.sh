cat <<'EOF' > expected
0 0
1 1
2 2
3 3
4 4
EOF

test_diff <<'EOF'
int printf();

int main() {
    int i = 1000;
    int j = 1000;
    for (i = 0, j = 0; i < 5; i++, j++) {
        printf("%d %d\n", i, j);
    }
}
EOF

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

