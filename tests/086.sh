cat <<'EOF' > expected
105
0
EOF

test_diff <<'EOF'
int printf();

int main() {
    char* s = "hi";
    while (*s++) {
        printf("%d\n", *s);
    }
}
EOF
