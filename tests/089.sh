set -e

cat <<'EOF' > expected
8
336
EOF
bash ../../test_diff.sh <<'EOF'
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
bash ../../test_diff.sh <<'EOF'
int printf();
int main() {
    printf("%d\n", 111 >> 3);
    printf("%d\n", 15 >> 14);
}
EOF
