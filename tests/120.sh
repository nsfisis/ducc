cat <<'EOF' > expected
42
123
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    // TODO: check if "foo" is expanded to "foo" or not once '#' operator is implemented.
#define foo foo
    int foo = 42;
    printf("%d\n", foo);

    // TODO: check if "a" is expanded to "a" or not once '#' operator is implemented.
#define a b
#define b c
#define c a
    int a = 123;
    printf("%d\n", a);
}
EOF
