set -e

bash ../../test_exit_code.sh 45 <<'EOF'
int main() {
    int i;
    int ret;
    i = 0;
    ret = 0;
    for (i = 0; i < 10; i = i + 1) {
        ret = ret + i;
    }
    return ret;
}
EOF
