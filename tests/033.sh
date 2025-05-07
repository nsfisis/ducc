set -e

cat <<'EOF' > expected
42
EOF
bash ../../test_diff.sh <<'EOF'
int printf();
void* calloc();

int main() {
    int b;
    int* a = &b;
    a[0] = 42;
    printf("%d\n", *a);
    return 0;
}
EOF

cat <<'EOF' > expected
0 0
1 1
2 2
3 3
4 4
5 5
6 6
7 7
8 8
9 9
EOF
bash ../../test_diff.sh <<'EOF'
int printf();
void* calloc();

int main() {
    long* a = calloc(10, sizeof(long));
    long i = 0;
    for (i = 0; i < 10; i = i + 1) {
        a[i] = i;
    }
    for (i = 0; i < 10; i = i + 1) {
        printf("%d %d\n", *(a + i), a[i]);
    }
    return 0;
}
EOF
