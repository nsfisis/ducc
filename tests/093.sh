set -e

cat <<'EOF' > expected
42
246
221
EOF

bash ../../test_diff.sh <<'EOF'
int printf();

#define A(x) x
#define B(x) x+x
#define C(x, y) x*y

int main() {
    printf("%d\n", A(42));
    printf("%d\n", B(123));
    printf("%d\n", C(13, 17));
}
EOF
