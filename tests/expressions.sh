# logical operators: short-circuit evaluation
cat <<'EOF' > expected
foo
EOF
test_diff <<'EOF'
int printf();

int foo() {
    printf("foo\n");
    return 0;
}

int bar() {
    printf("bar\n");
    return 1;
}

int main() {
    if (foo() && bar()) {
        printf("baz\n");
    }

    return 0;
}
EOF

cat <<'EOF' > expected
foo
bar
baz
EOF
test_diff <<'EOF'
int printf();

int foo() {
    printf("foo\n");
    return 0;
}

int bar() {
    printf("bar\n");
    return 1;
}

int main() {
    if (foo() || bar()) {
        printf("baz\n");
    }

    return 0;
}
EOF

# cast with typedef
cat <<'EOF' > expected
Result: -42
Result: 0
EOF
test_diff <<'EOF'
int printf(const char*, ...);

typedef int foo;

int main() {
    int a = 42;
    int b = -(int)a;
    int c = !(foo)a;
    printf("Result: %d\n", b);
    printf("Result: %d\n", c);
}
EOF

# comma operator in for loop
cat <<'EOF' > expected
0 0
1 1
2 2
3 3
4 4
EOF

test_diff <<'EOF'
int printf();

int main() {
    int i = 1000;
    int j = 1000;
    for (i = 0, j = 0; i < 5; i++, j++) {
        printf("%d %d\n", i, j);
    }
}
EOF

# void cast
cat <<'EOF' > expected
42
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int f() {
    return printf("42\n");
}

int main() {
    (void)123;
    (void)(5 + 6 + 7);
    (void)f();
}
EOF
