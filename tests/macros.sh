cat <<'EOF' > expected
EOF
test_diff <<'EOF'
#define A 1
int main() {
    return 0;
}
EOF

cat <<'EOF' > expected
1,2,3
EOF
test_diff <<'EOF'
int printf();

#define A 1
#define B 2
#define C 3

int main() {
    printf("%d,%d,%d\n", A, B, C);
    return 0;
}
EOF

cat <<'EOF' > expected
0,0,0,0
EOF
test_diff <<'EOF'
int printf();

#define NULL 0
#define TK_EOF        0
#define TY_UNKNOWN 0
#define AST_UNKNOWN       0

int main() {
    printf("%d,%d,%d,%d\n", NULL, TK_EOF, TY_UNKNOWN, AST_UNKNOWN);
    return 0;
}
EOF

cat <<'EOF' > expected
42,123
EOF
test_diff <<'EOF'
int printf();

#define A foo_a
#define B foo_b

int main() {
    int foo_a = 42;
    int foo_b = 123;

    printf("%d,%d\n", A, B);

    return 0;
}
EOF

cat <<'EOF' > expected
123
EOF

test_diff <<'EOF'
#define foo() 123

int printf();

int main() {
    printf("%d\n", foo());
}
EOF

cat <<'EOF' > expected
579
975
EOF

test_diff <<'EOF'
#define foo 123 + 456
#define bar() 321 + 654

int printf();

int main() {
    printf("%d\n", foo);
    printf("%d\n", bar());
}
EOF

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

cat <<'EOF' > expected
int printf ();


int foobar = 100;
int prefix_test = 200;
int test_suffix = 300;

int var_1 = 10;
int var_2 = 20;

int var_A = 30;

int number_12 = 12;

int bazqux = 42;

int main () {
 printf ( foobar=%d\n, foobar);
 printf ( prefix_test=%d\n, prefix_test);
 printf ( test_suffix=%d\n, test_suffix);
 printf ( var_1=%d\n, var_1);
 printf ( var_2=%d\n, var_2);
 printf ( var_A=%d\n, var_A);
 printf ( number_12=%d\n, number_12);
 return 0;
}
EOF

test_cpp <<'EOF'
int printf();

#define CONCAT(a, b) a##b
#define PREFIX(name) prefix_##name
#define SUFFIX(name) name##_suffix

int CONCAT(foo, bar) = 100;
int PREFIX(test) = 200;
int SUFFIX(test) = 300;

#define MAKE_VAR(n) var_##n
int MAKE_VAR(1) = 10;
int MAKE_VAR(2) = 20;

#define A 0
int MAKE_VAR(A) = 30;

#define NUMBER(x, y) number_##x##y
int NUMBER(1, 2) = 12;

#define CONCAT2(a, b) a ## b
int CONCAT2(baz, qux) = 42;

int main() {
    printf("foobar=%d\n", foobar);
    printf("prefix_test=%d\n", prefix_test);
    printf("test_suffix=%d\n", test_suffix);
    printf("var_1=%d\n", var_1);
    printf("var_2=%d\n", var_2);
    printf("var_A=%d\n", var_A);
    printf("number_12=%d\n", number_12);
    return 0;
}
EOF

cat <<'EOF' > expected
int printf ( const char *, ...);


int H ( int n) { return n; }

int main () {
 printf ( %d\n, H ( 123));
}
EOF

test_cpp <<'EOF'
int printf(const char*, ...);

#define F(x) CHECK(G(x))
#define G(x) CHECK(H(x))
#define CHECK(x) x

int H(int n) { return n; }

int main() {
    printf("%d\n", F(123));
}
EOF

test_exit_code 0 <<'EOF'
#include <helpers.h>

int strcmp(const char*, const char*);

#define FOO 42

#define TO_STRING(x) TO_STRING_HELPER(x)
#define TO_STRING_HELPER(x) #x
#define BAR TO_STRING(FOO)

#define TO_STRING2(x) #x
#define BAZ TO_STRING2(FOO)

int main() {
    ASSERT_EQ(0, strcmp("42", BAR));
    ASSERT_EQ(0, strcmp("FOO", BAZ));
}
EOF

test_exit_code 0 <<'EOF'
#include <helpers.h>

#define CONCAT3(x, y, z) x##y##z
#define FOO CONCAT3(, 2, 3)
#define BAR CONCAT3(1, , 3)
#define BAZ CONCAT3(1, 2, )

int main() {
    ASSERT_EQ(23, FOO);
    ASSERT_EQ(13, BAR);
    ASSERT_EQ(12, BAZ);
}
EOF

cat <<'EOF' > expected
foo
EOF

test_cpp <<'EOF'
#define CONCAT(x, y) x ## y
#define CONCAT2(name, r) CONCAT(name, r)
CONCAT2(foo,)
EOF

cat <<'EOF' > expected
1
EOF

test_diff <<'EOF'
int printf();

int main() {
    printf("%d\n", __ducc__);
    return 0;
}
EOF

cat <<'EOF' > expected
main.c
main.c
main.c
EOF

cat <<'EOF' > header.h
#define A __FILE__
EOF

test_diff <<'EOF'
#define B __FILE__
#include "header.h"
int printf();
int main() {
    printf("%s\n", __FILE__);
    printf("%s\n", B);
    printf("%s\n", A);
}
EOF

cat <<'EOF' > expected
5
6 6
7 7
EOF

cat <<'EOF' > header.h
#define A __LINE__
EOF

test_diff <<'EOF'
#define B __LINE__
#include "header.h"
int printf();
int main() {
    printf("%d\n", __LINE__);
    printf("%d %d\n", B, B);
    printf("%d %d\n", A, A);
}
EOF

cat <<'EOF' > expected
int printf ();

int main () {

 printf ( A is defined\n);


 printf ( A is undefined\n);
}
EOF

test_cpp <<'EOF'
int printf();

int main() {
#define A 1

#ifdef A
    printf("A is defined\n");
#else
    printf("A is undefined\n");
#endif

#undef A

#ifdef A
    printf("A is defined\n");
#else
    printf("A is undefined\n");
#endif
}
EOF
