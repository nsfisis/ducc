set -e

cat <<'EOF' > expected
foo
EOF
bash ../../test_diff.sh <<'EOF'
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
bash ../../test_diff.sh <<'EOF'
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
