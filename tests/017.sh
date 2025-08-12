set -e

touch expected
bash ../../test_diff.sh <<'EOF'
int main() {
    return 0;
}
EOF
