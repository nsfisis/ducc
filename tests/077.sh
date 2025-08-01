set -e

cat <<'EOF' > expected
0
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    printf("%d\n", '\0');
}
EOF
