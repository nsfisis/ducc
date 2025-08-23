test_exit_code 30 <<'EOF'
int main() {
    return (-10 + 20 * -3) + 100;
}
EOF
