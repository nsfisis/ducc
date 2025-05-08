set -e

cat <<'EOF' > expected
42
EOF
bash ../../test_diff.sh <<'EOF'
void* calloc();
int printf();

struct S {
    int x;
};
typedef struct S S;

int main() {
    S* s = calloc(1, sizeof(S));
    s->x = 42;
    printf("%d\n", s->x);
    return 0;
}
EOF
