set -e

cat <<'EOF' > expected
hello, world
EOF
bash ../../test_diff.sh <<'EOF'
int printf();
int main() {
    printf("hello, world\n");
    return 0;
}
EOF
