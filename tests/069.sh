cat <<'EOF' > expected
h
l
,
EOF

test_diff <<'EOF'
int printf();

int main() {
    char* h = " hello,world" + 1;
    printf("%c\n", *h);
    printf("%c\n", h[2]);
    printf("%c\n", *(h + 5));
}
EOF
