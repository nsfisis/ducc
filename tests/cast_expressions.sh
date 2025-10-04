cat <<'EOF' > expected
65
65
127
1
42
99
10
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    char c = 65;
    int i = (int)c;
    printf("%d\n", i);

    int i2 = 321;
    char c2 = (char)i2;
    printf("%d\n", c2);

    short s = 127;
    int i3 = (int)s;
    printf("%d\n", i3);

    int i4 = 65537;
    short s2 = (short)i4;
    printf("%d\n", s2);

    long l = 42;
    int i5 = (int)l;
    printf("%d\n", i5);

    int i6 = 99;
    long l2 = (long)i6;
    printf("%d\n", (int)l2);

    char c3 = 10;
    short s3 = (short)c3;
    int i7 = (int)s3;
    long l3 = (long)i7;
    printf("%d\n", (int)l3);

    return 0;
}
EOF

cat <<'EOF' > expected
Result: 130
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    char c = 65;
    int result = (int)c + (int)c;
    printf("Result: %d\n", result);
    return 0;
}
EOF

cat <<'EOF' > expected
10
20
30
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    char a = 5;
    char b = 5;
    int sum = (int)a + (int)b;
    printf("%d\n", sum);

    short s1 = 10;
    short s2 = 10;
    int sum2 = (int)s1 + (int)s2;
    printf("%d\n", sum2);

    long l1 = 15;
    long l2 = 15;
    int sum3 = (int)(l1 + l2);
    printf("%d\n", sum3);

    return 0;
}
EOF

cat <<'EOF' > expected
10
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    char c = -10;
    int i = (int)c;
    printf("%d\n", -i);
    return 0;
}
EOF

cat <<'EOF' > expected
Char: 65
Int: 65
EOF
test_diff <<'EOF'
int printf(const char*, ...);

char get_char() {
    return 65;
}

int main() {
    char c = get_char();
    int i = (int)get_char();
    printf("Char: %d\n", c);
    printf("Int: %d\n", i);
    return 0;
}
EOF

cat <<'EOF' > expected
Equal
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    char c = 42;
    int i = 42;
    if ((int)c == i) {
        printf("Equal\n");
    } else {
        printf("Not equal\n");
    }
    return 0;
}
EOF

cat <<'EOF' > expected
55
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    long l = 55;
    char c = (char)(short)(int)l;
    printf("%d\n", c);
    return 0;
}
EOF
