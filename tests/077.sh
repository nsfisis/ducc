cat <<'EOF' > expected
0
EOF
test_diff <<'EOF'
int printf();

int main() {
    printf("%d\n", '\0');
}
EOF
