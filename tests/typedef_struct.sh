cat <<'EOF' > expected
42
EOF
test_diff <<'EOF'
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

cat <<'EOF' > expected
4
EOF
test_diff <<'EOF'
struct S0;
typedef struct S0 S0;

struct S1 {
    int x;
};

struct S2 {
    int x;
};

struct S3 {
    int x;
};

struct S4 {
    int x;
};

struct S0 {
    int x;
};

int printf(const char*, ...);
int main() {
    printf("%zu\n", sizeof(S0));
}
EOF

cat <<'EOF' > expected
8
4
4
EOF

test_diff <<'EOF'
int printf();

typedef struct {
    int x;
    int y;
} S;

typedef union {
    int a;
    char b;
} U;

typedef enum {
    RED,
    GREEN,
    BLUE
} E;

int main() {
    printf("%zu\n", sizeof(S));
    printf("%zu\n", sizeof(U));
    printf("%zu\n", sizeof(E));
}
EOF
