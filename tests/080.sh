set -e

cat <<'EOF' > expected
44
44
46
46
44
42
42
EOF

bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    int a = 42;
    ++a;
    a++;
    printf("%d\n", a);
    printf("%d\n", a++);
    printf("%d\n", ++a);
    printf("%d\n", a);
    --a;
    a--;
    printf("%d\n", a--);
    printf("%d\n", --a);
    printf("%d\n", a);
}
EOF
