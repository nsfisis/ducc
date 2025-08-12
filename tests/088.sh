set -e

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

bash ../../test_diff.sh <<'EOF'
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
