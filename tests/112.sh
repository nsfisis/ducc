cat <<'EOF' > expected
42
100 200
300
0
1 2 3
15
42
123
879
EOF

test_diff <<'EOF'
int printf();

#define ADD(a, b) ((a) + (b))
#define PRINT_TWO(x, y) printf("%d %d\n", x, y)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NESTED(x) (x)
#define CONCAT3(a, b, c) a ## b ## c

int main() {
    printf("%d\n", ADD(40, 2));
    PRINT_TWO(100, 200);
    printf("%d\n", MAX(100 + 200, 250));
    printf("%d\n");
    NESTED((printf("1 "), printf("2 "), printf("3\n"), 0));

    int x = 5, y = 10;
    printf("%d\n", ADD(x + 2, y - 2));
    printf("%d\n", ADD(MAX(10, 20), MAX(15 + 5, 22)));
    printf("%d\n", ADD(  100  ,  23  ));
    printf("%d\n", ADD(NESTED((100 + 200)), MAX((123 + 456), (111 + 222))));
}
EOF
