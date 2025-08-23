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
