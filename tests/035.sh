set -e

cat <<'EOF' > expected
0
1
0
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    printf("%d\n", !1);
    printf("%d\n", !0);
    printf("%d\n", !23);
    return 0;
}
EOF
