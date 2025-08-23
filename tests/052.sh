cat <<'EOF' > expected
EOF
test_diff <<'EOF'
// TODO: check if the token is at the beginning of line.
// TODO: check if skipped whitespaces do not contain line breaks.
int main() {
    return 0;
}
EOF
