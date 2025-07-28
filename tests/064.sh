set -e

# C99: 5.1.2.2.3
bash ../../test_exit_code.sh 0 <<'EOF'
int main() {
}
EOF

bash ../../test_exit_code.sh 0 <<'EOF'
int main() {
    1 + 2 + 3;
}
EOF

bash ../../test_exit_code.sh 0 <<'EOF'
int main() {
    if (1);
    else return 1;
}
EOF
