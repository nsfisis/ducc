set -e

cat <<'EOF' > expected
hi
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

void f() {
    printf("hi\n");
    return;
}

int main() {
    f();
    return 0;
}
EOF
