cat <<'EOF' > expected
42
123
EOF
test_diff <<'EOF'
struct S {
    int a;
    int b;
};

int printf();
void* calloc();

int main() {
    struct S* sp;
    sp = calloc(1, sizeof(struct S));
    sp->a = 42;
    printf("%d\n", sp->a);
    (*sp).b = 123;
    printf("%d\n", (*sp).b);
    return 0;
}
EOF
