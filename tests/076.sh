set -e

cat <<'EOF' > expected
sizeof(a) = 4
sizeof(b) = 8
sizeof(c) = 123
sizeof(d) = 16
sizeof(e) = 8
sizeof(f) = 1968
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

struct S {
    long x;
    long y;
};

int main() {
    int a;
    long b;
    char c[123];
    struct S d;
    void* e;
    struct S f[123];

    printf("sizeof(a) = %d\n", sizeof(a));
    printf("sizeof(b) = %d\n", sizeof(b));
    printf("sizeof(c) = %d\n", sizeof(c));
    printf("sizeof(d) = %d\n", sizeof(d));
    printf("sizeof(e) = %d\n", sizeof(e));
    printf("sizeof(f) = %d\n", sizeof(f));
    return 0;
}
EOF
