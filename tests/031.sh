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
