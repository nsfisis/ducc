set -e

bash ../../test_exit_code.sh 30 <<'EOF'
int main() {
    return (-10 + 20 * -3) + 100;
}
EOF
