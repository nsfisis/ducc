set -e

cat <<'EOF' > expected
1 2 3 4
EOF

bash ../../test_diff.sh <<'EOF'
int printf();
int main() {
    int a, b;
    a = 1, b = 2;
    int c = 3, d = 4;
    printf("%d %d %d %d\n", a, b, c, d);
}
EOF
