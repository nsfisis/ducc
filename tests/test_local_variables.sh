test_exit_code 42 <<'EOF'
int main() {
    int foo;
    foo = 42;
    return foo;
}
EOF

test_exit_code 70 <<'EOF'
int main() {
    int foo;
    int bar;
    foo = 42;
    bar = 28;
    return foo + bar;
}
EOF

test_exit_code 45 <<'EOF'
int main() {
    int a1;
    int a2;
    int a3;
    int a4;
    int a5;
    int a6;
    int a7;
    int a8;
    int a9;

    a1 = 1;
    a2 = 2;
    a3 = 3;
    a4 = 4;
    a5 = 5;
    a6 = 6;
    a7 = 7;
    a8 = 8;
    a9 = 9;

    return
        a1 +
        a2 +
        a3 +
        a4 +
        a5 +
        a6 +
        a7 +
        a8 +
        a9 +
        0;
}
EOF

cat <<'EOF' > expected
42
EOF
test_diff <<'EOF'
int printf();

int main() {
    int a = 42;
    printf("%d\n", a);
    return 0;
}
EOF

