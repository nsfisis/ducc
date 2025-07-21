set -e

cat <<'EOF' > expected
1
EOF

bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    printf("%d\n", __ducc__);
    return 0;
}
EOF
