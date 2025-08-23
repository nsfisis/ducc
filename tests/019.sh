touch expected
test_diff <<'EOF'
int printf();

int main() {
    printf("");
    return 0;
}
EOF

cat <<'EOF' > expected
Hello, World!
EOF
test_diff <<'EOF'
int printf();

int main() {
    printf("Hello, World!\n");
    return 0;
}
EOF

cat <<'EOF' > expected
"Hello, World!"
EOF
test_diff <<'EOF'
int printf();

int main() {
    printf("\"Hello, World!\"\n");
    return 0;
}
EOF
