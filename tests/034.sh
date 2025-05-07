set -e

cat <<'EOF' > expected
EOF
bash ../../test_diff.sh <<'EOF'
struct S {
    int a;
};

struct S* f();

struct S* g() {}

int main() {
    return 0;
}
EOF
