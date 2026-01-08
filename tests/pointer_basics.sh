cat <<'EOF' > expected
EOF
test_diff <<'EOF'
int main() {
    int a1;
    int* a2;
    char a3;
    char* a4;
    long a5;
    long* a6;
    void* a8;
    int** a10;
    char** a12;
    long** a14;
    void** a16;
    int*** a18;
    char*** a20;
    long*** a22;
    void*** a24;
    int* const* const* a25;
    return 0;
}
EOF

cat <<'EOF' > expected
42 42
EOF
test_diff <<'EOF'
int printf();

int main() {
    int x;
    int* y;
    y = &x;
    *y = 42;
    printf("%d %d\n", x, *y);
    return 0;
}
EOF

cat <<'EOF' > expected
3
3
3
EOF
test_diff <<'EOF'
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
