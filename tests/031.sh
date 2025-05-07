set -e

cat <<'EOF' > expected
42
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    int a = 42;
    printf("%d\n", a);
    return 0;
}
EOF
