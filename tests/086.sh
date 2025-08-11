set -e

cat <<'EOF' > expected
105
0
EOF

bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    char* s = "hi";
    while (*s++) {
        printf("%d\n", *s);
    }
}
EOF
