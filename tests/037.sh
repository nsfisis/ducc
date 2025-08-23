cat <<'EOF' > expected
hi
EOF
test_diff <<'EOF'
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
