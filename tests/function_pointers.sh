cat <<'EOF' > expected
a
h
g
EOF

test_diff <<'EOF'
int* f1(int a);
int (*f2)(int a);

extern int atexit (void (*) (void));
extern int atexit (void (*fn) (void));

int printf(const char*, ...);

void g() { printf("g\n"); }
void h() { printf("h\n"); }

int main() {
    atexit(g);
    atexit(h);
    printf("a\n");
}
EOF
