set -e

cat <<'EOF' > expected
42
EOF
bash ../../test_diff.sh <<'EOF'
void* calloc();
int printf();

struct T;

struct S {
    struct T* a;
};

struct T {
    int b;
};

int main() {
    struct S* s = calloc(1, sizeof(struct S));
    s->a = calloc(1, sizeof(struct T));
    s->a->b = 42;
    printf("%d\n", s->a->b);
    return 0;
}
EOF
