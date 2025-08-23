cat <<'EOF' > expected
0 0
1 1
2 2
3 3
4 4
EOF

test_diff <<'EOF'
int printf();

int main() {
    int i = 1000;
    int j = 1000;
    for (i = 0, j = 0; i < 5; i++, j++) {
        printf("%d %d\n", i, j);
    }
}
EOF
