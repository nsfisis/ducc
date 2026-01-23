# pointer basics
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

# arrays
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

# string operations
> expected
test_diff <<'EOF'
int main() {
    "";
    return 0;
}
EOF

> expected
test_diff <<'EOF'
int main() {
    "abc";
    return 0;
}
EOF

> expected
test_diff <<'EOF'
int main() {
    "\"foo\"bar\\\n\"";
    return 0;
}
EOF

cat <<'EOF' > expected
abc
defghijkl
EOF

test_diff <<'EOF'
int printf();

int main() {
    printf("abc\n");
    printf("def" "ghi"
    "jkl\n");
}
EOF

cat <<'EOF' > expected
h
l
,
EOF

test_diff <<'EOF'
int printf();

int main() {
    char* h = " hello,world" + 1;
    printf("%c\n", *h);
    printf("%c\n", h[2]);
    printf("%c\n", *(h + 5));
}
EOF

cat <<'EOF' > expected
105
0
EOF

test_diff <<'EOF'
int printf();

int main() {
    char* s = "hi";
    while (*s++) {
        printf("%d\n", *s);
    }
}
EOF
