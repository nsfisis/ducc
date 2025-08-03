set -e

cat <<'EOF' > expected
8
42
42
EOF

bash ../../test_diff.sh <<'EOF'
int printf();

union U {
    int i;
    long l;
};

int main() {
    union U u;
    printf("%zu\n", sizeof(u));
    u.l = 42;
    printf("%d\n", u.i);
    printf("%ld\n", u.l);
}
EOF
