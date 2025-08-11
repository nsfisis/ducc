set -e

cat <<'EOF' > expected
0
42
48
0
0
0
0
0
0
0
0
0
0
0
0
123
EOF

bash ../../test_diff.sh <<'EOF'
int printf();

int a;
int b[12];

int main() {
    printf("%d\n", a);
    a = 42;
    printf("%d\n", a);
    printf("%zu\n", sizeof(b));
    int i;
    for (i = 0; i < 12; ++i) {
        printf("%d\n", b[i]);
    }
    b[11] = 123;
    printf("%d\n", b[11]);
}
EOF
