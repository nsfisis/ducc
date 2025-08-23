test_exit_code 197 <<'EOF'
int main() {
    return (((3+5)/2) + (5*(9-6)) * (5+6*7)) % 256;
}
EOF
