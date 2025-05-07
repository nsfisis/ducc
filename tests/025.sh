set -e

cat <<'EOF' > expected
123
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

void foo_bar(int hoge_piyo) {
    printf("%d\n", hoge_piyo);
}

int main() {
    foo_bar(123);
    return 0;
}
EOF
