cat <<'EOF' > expected
2 5
EOF

test_diff <<'EOF'
int printf();

int main() {
    printf("%d %d\n", 1 ? 2 : 3, 0 ? 4 : 5);
}
EOF

