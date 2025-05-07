set -e

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
EOF
bash ../../test_diff.sh <<'EOF'
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

    return 0;
}
EOF
