test_exit_code 70 <<'EOF'
int main() {
    int foo;
    int bar;
    foo = 42;
    bar = 28;
    return foo + bar;
}
EOF
