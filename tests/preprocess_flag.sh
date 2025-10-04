cat <<'EOF' > expected

int main () {
 int x = 100;
 int y = 0;
 return 0;
}
EOF

test_cpp <<'EOF'
#define MAX 100
#define MIN 0

int main() {
    int x = MAX;
    int y = MIN;
    return 0;
}
EOF

cat <<'EOF' > expected

int main () {
 int sum = ( ( 10) + ( 20));
 int product = ( ( 3) * ( 4));
 return 0;
}
EOF

test_cpp <<'EOF'
#define ADD(a, b) ((a) + (b))
#define MUL(x, y) ((x) * (y))

int main() {
    int sum = ADD(10, 20);
    int product = MUL(3, 4);
    return 0;
}
EOF

cat <<'EOF' > expected
int main () {
 return 42;
}
EOF

test_cpp <<'EOF'
#define FOO 42
int main() {
    return FOO;
}
EOF

cat <<'EOF' > expected
int foo () { return 1; }
EOF

test_cpp <<'EOF'
#define X 1
#ifdef X
int foo() { return 1; }
#else
int foo() { return 0; }
#endif
EOF

cat <<'EOF' > expected

int main () {
 int x = 1 + 2 * 3;
 return 0;
}
EOF

test_cpp <<'EOF'
#define A 1
#define B A + 2
#define C B * 3

int main() {
    int x = C;
    return 0;
}
EOF

cat <<'EOF' > expected

int main () {
 int x = ( 5 + 3);
 int y = ( ( 5) + ( 3));
 return 0;
}
EOF

test_cpp <<'EOF'
#define SIMPLE (5 + 3)
#define COMPLEX(a, b) ((a) + (b))

int main() {
    int x = SIMPLE;
    int y = COMPLEX(5, 3);
    return 0;
}
EOF

cat <<'EOF' > expected

int main () {
 int bar = 0;
 bar ++;
}
EOF

test_cpp <<'EOF'
#define A(a) a
#define B(b) b

int main() {
    A(int)B(bar) = 0;
    bar++;
}
EOF
