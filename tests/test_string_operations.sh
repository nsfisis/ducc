touch expected
test_diff <<'EOF'
int main() {
    "";
    return 0;
}
EOF

touch expected
test_diff <<'EOF'
int main() {
    "abc";
    return 0;
}
EOF

touch expected
test_diff <<'EOF'
int main() {
    "\"foo\"bar\\\n\"";
    return 0;
}
EOF

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

cat <<'EOF' > expected
105
0
EOF

test_diff <<'EOF'
int printf();

int main() {
    char* s = "hi";
    while (*s++) {
        printf("%d\n", *s);
    }
}
EOF

