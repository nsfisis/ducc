set -e

cat <<'EOF' > expected
97 48
92 39
10
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    printf("%d %d\n", 'a', '0');
    printf("%d %d\n", '\\', '\'');
    printf("%d\n", '\n');
    return 0;
}
EOF
