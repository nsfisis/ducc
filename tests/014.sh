set -e

bash ../../test_exit_code.sh 66 <<'EOF'
int foo() {
    int i;
    int ret;
    i = 0;
    ret = 0;
    for (i = 0; i < 100; i = i + 1) {
        if (i == 12) {
            break;
        }
        ret = ret + i;
    }
    return ret;
}

int main() {
    return foo();
}
EOF
