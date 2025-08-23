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
