set -e

bash ../../test_output.sh "" <<'EOF'
int main() {
    "";
    return 0;
}
EOF

bash ../../test_output.sh "" <<'EOF'
int main() {
    "abc";
    return 0;
}
EOF

bash ../../test_output.sh "" <<'EOF'
int main() {
    "\"foo\"bar\\\n\"";
    return 0;
}
EOF
