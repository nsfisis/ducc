cat <<'EOF' > expected
hello, world
EOF
test_diff <<'EOF'
int printf();
int main() {
    printf("hello, world\n");
    return 0;
}
EOF
