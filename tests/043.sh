set -e

cat <<'EOF' > expected
42,123
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    int _a = 42;
    int _b = 123;

    printf("%d,%d\n", _a, _b);

    return 0;
}
EOF
