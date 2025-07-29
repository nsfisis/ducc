set -e

cat <<'EOF' > expected
0
1
4
9
16
25
36
49
64
81
EOF

bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    int i;
    int a[10];
    for (i = 0; i < 10; ++i) {
        a[i] = i * i;
    }
    for (i = 0; i < 10; ++i) {
        printf("%d\n", a[i]);
    }
}
EOF
