cat <<'EOF' > expected
42
EOF
test_diff <<'EOF'
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
test_diff <<'EOF'
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

cat <<'EOF' > expected
65
65
66
67
68
EOF
test_diff <<'EOF'
void* calloc();
int printf();

int main() {
    char* source = calloc(4, sizeof(char));

    source[0] = 'A';
    source[1] = 'B';
    source[2] = 'C';
    source[3] = 'D';

    int a = source[0];

    printf("%d\n", a);
    printf("%d\n", source[0]);
    printf("%d\n", source[1]);
    printf("%d\n", source[2]);
    printf("%d\n", source[3]);

    return 0;
}
EOF

cat <<'EOF' > expected
0
1
4
9
16
25
36
49
64
81
EOF

test_diff <<'EOF'
int printf();

int main() {
    int i;
    int a[10];
    for (i = 0; i < 10; ++i) {
        a[i] = i * i;
    }
    for (i = 0; i < 10; ++i) {
        printf("%d\n", a[i]);
    }
}
EOF

cat <<'EOF' > expected
0
42
48
0
0
0
0
0
0
0
0
0
0
0
0
123
EOF

test_diff <<'EOF'
int printf();

int a;
int b[12];

int main() {
    printf("%d\n", a);
    a = 42;
    printf("%d\n", a);
    printf("%zu\n", sizeof(b));
    int i;
    for (i = 0; i < 12; ++i) {
        printf("%d\n", b[i]);
    }
    b[11] = 123;
    printf("%d\n", b[11]);
}
EOF

cat <<'EOF' > expected
400
80
16
EOF

test_diff <<'EOF'
int printf();

int main() {
    int a[10 * 10];
    int b[10 + 10];
    int c[1 << 2];

    printf("%zu\n", sizeof(a));
    printf("%zu\n", sizeof(b));
    printf("%zu\n", sizeof(c));
}
EOF
