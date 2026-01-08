cat <<'EOF' > expected
0
1
EOF
test_diff <<'EOF'
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

cat <<'EOF' > expected
123 456
0 0
123 456
EOF

test_diff <<'EOF'
struct S {
    int a;
    int b;
};
typedef struct S S;

void* calloc();
int printf();

int main() {
    S* s1 = calloc(1, sizeof(S));
    S* s2 = calloc(1, sizeof(S));
    s1->a = 123;
    s1->b = 456;
    printf("%d %d\n", s1->a, s1->b);
    printf("%d %d\n", s2->a, s2->b);
    *s2 = *s1;
    printf("%d %d\n", s2->a, s2->b);
}
EOF

cat <<'EOF' > expected
123 456
0 0
123 456
EOF

test_diff <<'EOF'
struct S {
    long a;
    long b;
};
typedef struct S S;

void* calloc();
int printf();

int main() {
    S* s1 = calloc(1, sizeof(S));
    S* s2 = calloc(1, sizeof(S));
    s1->a = 123;
    s1->b = 456;
    printf("%d %d\n", s1->a, s1->b);
    printf("%d %d\n", s2->a, s2->b);
    *s2 = *s1;
    printf("%d %d\n", s2->a, s2->b);
}
EOF

cat <<'EOF' > expected
123 456
0 0
123 456
EOF

test_diff <<'EOF'
struct S {
    long a;
    long b;
};
typedef struct S S;

void* calloc();
int printf();

int main() {
    S s1;
    S s2;
    s1.a = 123;
    s1.b = 456;
    s2.a = 0;
    s2.b = 0;
    printf("%d %d\n", s1.a, s1.b);
    printf("%d %d\n", s2.a, s2.b);
    s2 = s1;
    printf("%d %d\n", s2.a, s2.b);
}
EOF
