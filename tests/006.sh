set -e

bash ../../test_exit_code.sh 1 <<'EOF'
int main() {
    return 0 == 0;
}
EOF

bash ../../test_exit_code.sh 0 <<'EOF'
int main() {
    return 123 != 123;
}
EOF

bash ../../test_exit_code.sh 1 <<'EOF'
int main() {
    return 123 != 456;
}
EOF

bash ../../test_exit_code.sh 0 <<'EOF'
int main() {
    return 123 == 124;
}
EOF

bash ../../test_exit_code.sh 1 <<'EOF'
int main() {
    return 123 < 567;
}
EOF

bash ../../test_exit_code.sh 1 <<'EOF'
int main() {
    return 123 <= 567;
}
EOF

bash ../../test_exit_code.sh 1 <<'EOF'
int main() {
    return 123 <= 123;
}
EOF

bash ../../test_exit_code.sh 0 <<'EOF'
int main() {
    return 123 < 123;
}
EOF
