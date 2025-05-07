set -e

bash ../../test_output.sh "" <<'EOF'
int printf();

int main() {
    printf("");
    return 0;
}
EOF

bash ../../test_output.sh "Hello, World!" <<'EOF'
int printf();

int main() {
    printf("Hello, World!\n");
    return 0;
}
EOF

bash ../../test_output.sh '"Hello, World!"' <<'EOF'
int printf();

int main() {
    printf("\"Hello, World!\"\n");
    return 0;
}
EOF
