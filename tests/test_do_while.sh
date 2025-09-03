cat <<'EOF' > expected
body 0
foo 1
body 1
foo 2
body 2
foo 3
body 3
foo 4
body 4
foo 5
EOF
test_diff <<'EOF'
int printf();

int foo(int i) {
    printf("foo %d\n", i);
    return i;
}

int main() {
    int i = 0;
    do {
        printf("body %d\n", i);
        ++i;
    } while (foo(i) < 5);

    return 0;
}
EOF

cat <<'EOF' > expected
body 0
foo 1
body 1
foo 2
body 2
EOF
test_diff <<'EOF'
int printf();

int foo(int i) {
    printf("foo %d\n", i);
    return i;
}

int main() {
    int i = 0;
    do {
        printf("body %d\n", i);
        ++i;
        if (i == 3) {
            break;
        }
    } while (foo(i) < 5);

    return 0;
}
EOF

cat <<'EOF' > expected
body 1
foo 1
foo 2
body 3
foo 3
foo 4
body 5
foo 5
EOF
test_diff <<'EOF'
int printf();

int foo(int i) {
    printf("foo %d\n", i);
    return i;
}

int main() {
    int i = 0;
    do {
        ++i;
        if (i % 2 == 0) {
            continue;
        }
        printf("body %d\n", i);
    } while (foo(i) < 5);

    return 0;
}
EOF

