cat <<'EOF' > expected
42 42
EOF
test_diff <<'EOF'
int printf();

int main() {
    int x;
    int* y;
    y = &x;
    *y = 42;
    printf("%d %d\n", x, *y);
    return 0;
}
EOF
