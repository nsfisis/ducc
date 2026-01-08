cat <<'EOF' > expected
123
EOF
test_diff <<'EOF'
int printf();

void foo_bar(int hoge_piyo) {
    printf("%d\n", hoge_piyo);
}

int main() {
    foo_bar(123);
    return 0;
}
EOF

cat <<'EOF' > expected
EOF
test_diff <<'EOF'
struct S {
    int a;
};

struct S* f();

struct S* g() {}

int main() {
    return 0;
}
EOF

cat <<'EOF' > expected
hi
EOF
test_diff <<'EOF'
int printf();

void f() {
    printf("hi\n");
    return;
}

int main() {
    f();
    return 0;
}
EOF
