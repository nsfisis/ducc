bash ../../test_exit_code.sh 21 <<'EOF'
int main() {
    return 5+20-4;
}
EOF
