set -e

cat <<'EOF' > expected
3
3
3
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    char c;
    int i;
    long l;
    c = 42;
    i = 42*2;
    l = 42*3;

    char* cp1;
    char* cp2;
    int* ip1;
    int* ip2;
    long* lp1;
    long* lp2;

    cp1 = &c;
    cp2 = &c + 3;

    ip1 = &i;
    ip2 = &i + 3;

    lp1 = &l;
    lp2 = &l + 3;

    printf("%d\n", cp2 - cp1);
    printf("%d\n", ip2 - ip1);
    printf("%d\n", lp2 - lp1);

    return 0;
}
EOF
