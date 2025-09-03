test_exit_code 45 <<'EOF'
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

test_exit_code 25 <<'EOF'
int main() {
    int i;
    int ret;
    i = 0;
    ret = 0;
    for (i = 0; i < 10; i = i + 1) {
        if (i % 2 == 0) {
            continue;
        }
        ret = ret + i;
    }
    return ret;
}
EOF

test_exit_code 66 <<'EOF'
int main() {
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
EOF

cat <<'EOF' > expected
0
1
2
3
4
10
11
12
13
14
20
21
22
23
24
25
EOF
test_diff <<'EOF'
int printf();

int main() {
    int i = 0;
    for (; i < 5; i = i + 1) {
        printf("%d\n", i);
    }
    for (i = 10; i < 15; ) {
        printf("%d\n", i);
        i = i + 1;
    }
    for (i = 20; ; i = i + 1) {
        printf("%d\n", i);
        if (i == 25) break;
    }

    return 0;
}
EOF

cat <<'EOF' > expected
0
1
2
3
4
5
6
7
8
9
EOF

test_diff <<'EOF'
int printf();

int main() {
    for (int i = 0; i < 10; i++) {
        printf("%d\n", i);
    }
}
EOF

cat <<'EOF' > expected
0
1
2
3
4
0
1
2
3
4
43
42
EOF

test_diff <<'EOF'
int printf();

int main() {
    for (int i = 0; i < 5; i++) {
        printf("%d\n", i);
    }
    for (int i = 0; i < 5; i++) {
        printf("%d\n", i);
    }
    int x = 42;
    {
        int x = 43;
        printf("%d\n", x);
    }
    printf("%d\n", x);
}
EOF

