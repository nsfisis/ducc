cat <<'EOF' > expected
0
1
2
3
4

5
4
3
2
1
0

56088

24

3
EOF
test_diff <<'EOF'
int printf();

int main() {
    int i = 0;
    for (; i < 5; i += 1) {
        printf("%d\n", i);
    }
    printf("\n");
    for (i = 5; i >= 0; i -= 1) {
        printf("%d\n", i);
    }

    int x = 123;
    x *= 456;
    printf("\n%d\n", x);

    int y = 120;
    y /= 5;
    printf("\n%d\n", y);

    int z = 17;
    z %= 7;
    printf("\n%d\n", z);

    return 0;
}
EOF
