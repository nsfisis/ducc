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
