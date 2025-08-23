cat <<'EOF' > expected
undefined function: f
EOF
test_compile_error <<'EOF'
int main() {
    f();
}
EOF
