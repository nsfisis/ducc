set -e

bash ../../test_exit_code.sh 89 <<'EOF'
int fib(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}

int main() {
    return fib(10);
}
EOF
