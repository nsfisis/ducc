set -e

bash ../../test_exit_code.sh 42 <<'EOF'
int main() {
    int foo;
    foo = 42;
    return foo;
}
EOF
