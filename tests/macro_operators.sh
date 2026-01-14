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
