cat <<'EOF' > expected
1,0
1,1,1
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    bool b1 = true, b0 = false;
    printf("%d,%d\n", b1, b0);
    printf("%d,%d,%d\n", sizeof(b1), sizeof(b0), sizeof(bool));
}
EOF
