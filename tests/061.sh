cat <<'EOF' > expected
1
EOF

test_diff <<'EOF'
int printf();

int main() {
    printf("%d\n", __ducc__);
    return 0;
}
EOF
