cat <<'EOF' > expected
EOF

test_diff <<'EOF'
int* f(int a);
int (*f)(int a);

extern int atexit (void (*) (void));
extern int atexit (void (*fn) (void));

int main() {}
EOF
