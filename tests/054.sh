set -e

cat <<'EOF' > expected
0
1
EOF
bash ../../test_diff.sh <<'EOF'
int printf();
void* calloc();

struct S {
    int a;
    int b;
};

int main() {
    struct S* s = calloc(1, sizeof(struct S));
    s->b = 1;
    printf("%ld\n", s->a);
    printf("%ld\n", s->b);
    return 0;
}
EOF
