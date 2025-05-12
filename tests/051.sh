set -e

cat <<'EOF' > expected
undefined function: f
EOF
bash ../../test_compile_error.sh <<'EOF'
int main() {
    f();
}
EOF
