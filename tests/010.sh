set -e

bash ../../test_exit_code.sh 12 <<'EOF'
int main() {
    if (1) {
        return 12;
    } else {
        return 34;
    }
}
EOF

bash ../../test_exit_code.sh 34 <<'EOF'
int main() {
    if (1 + 1 != 2) {
        return 12;
    } else {
        return 34;
    }
}
EOF
