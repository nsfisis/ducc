cat <<'EOF' > expected
20
10
20
30
40
50
EOF

test_diff <<'EOF'
int printf();

struct S {
    int a[5];
};

int main() {
    struct S x;
    x.a[0] = 10;
    x.a[1] = 20;
    x.a[2] = 30;
    x.a[3] = 40;
    x.a[4] = 50;

    printf("%zu\n", sizeof(struct S));

    printf("%d\n", x.a[0]);
    printf("%d\n", x.a[1]);
    printf("%d\n", x.a[2]);
    printf("%d\n", x.a[3]);
    printf("%d\n", x.a[4]);
}
EOF
