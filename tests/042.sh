set -e

cat <<'EOF' > expected
42,123
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

#define A foo_a
#define B foo_b

int main() {
    int foo_a = 42;
    int foo_b = 123;

    printf("%d,%d\n", A, B);

    return 0;
}
EOF
