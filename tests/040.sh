set -e

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
bash ../../test_diff.sh <<'EOF'
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
