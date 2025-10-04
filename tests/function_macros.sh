cat <<'EOF' > expected
42
246
221
EOF

test_diff <<'EOF'
int printf();

#define A(x) x
#define B(x) x+x
#define C(x, y) x*y

int main() {
    printf("%d\n", A(42));
    printf("%d\n", B
    (123));
    printf("%d\n", C (13, 17));
}
EOF

cat <<'EOF' > expected
123
EOF
test_diff <<'EOF'
#define A(x) x

int printf();

int main() {
    printf("%d\n", A ( 123 ));
}
EOF

cat <<'EOF' > expected
main.c:4: expected ')', but got '<new-line>'
EOF
test_compile_error <<'EOF'
#define A(x) x

int main() {
#if A (
       123) < A(345)
    printf("1\n");
#endif
}
EOF

cat <<'EOF' > expected
42
100 200
300
0
1 2 3
15
42
123
879
EOF

test_diff <<'EOF'
int printf();

#define ADD(a, b) ((a) + (b))
#define PRINT_TWO(x, y) printf("%d %d\n", x, y)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NESTED(x) (x)
#define CONCAT3(a, b, c) a ## b ## c

int main() {
    printf("%d\n", ADD(40, 2));
    PRINT_TWO(100, 200);
    printf("%d\n", MAX(100 + 200, 250));
    printf("%d\n");
    NESTED((printf("1 "), printf("2 "), printf("3\n"), 0));

    int x = 5, y = 10;
    printf("%d\n", ADD(x + 2, y - 2));
    printf("%d\n", ADD(MAX(10, 20), MAX(15 + 5, 22)));
    printf("%d\n", ADD(  100  ,  23  ));
    printf("%d\n", ADD(NESTED((100 + 200)), MAX((123 + 456), (111 + 222))));
}
EOF

cat <<'EOF' > expected
int printf ( const char *, ...);

int main () {
 int foo = 42;
 printf ( %d\n, foo);

 int a = 123;
 printf ( %d\n, a);
}
EOF
test_cpp <<'EOF'
int printf(const char*, ...);

int main() {
#define foo foo
    int foo = 42;
    printf("%d\n", foo);

#define a b
#define b c
#define c a
    int a = 123;
    printf("%d\n", a);
}
EOF
