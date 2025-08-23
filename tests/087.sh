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
