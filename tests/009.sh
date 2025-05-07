set -e

bash ../../test_exit_code.sh 45 <<'EOF'
int main() {
    int a1;
    int a2;
    int a3;
    int a4;
    int a5;
    int a6;
    int a7;
    int a8;
    int a9;

    a1 = 1;
    a2 = 2;
    a3 = 3;
    a4 = 4;
    a5 = 5;
    a6 = 6;
    a7 = 7;
    a8 = 8;
    a9 = 9;

    return
        a1 +
        a2 +
        a3 +
        a4 +
        a5 +
        a6 +
        a7 +
        a8 +
        a9 +
        0;
}
EOF
