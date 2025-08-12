set -e

touch expected
bash ../../test_diff.sh <<'EOF'
int main() {
    "";
    return 0;
}
EOF

touch expected
bash ../../test_diff.sh <<'EOF'
int main() {
    "abc";
    return 0;
}
EOF

touch expected
bash ../../test_diff.sh <<'EOF'
int main() {
    "\"foo\"bar\\\n\"";
    return 0;
}
EOF
