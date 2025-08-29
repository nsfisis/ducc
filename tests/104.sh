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
