cat <<'EOF' > expected
main.c:2: 'case' label not within a switch statement
EOF
test_compile_error <<'EOF'
int main() {
    case 1:
        return 0;
}
EOF

cat <<'EOF' > expected
main.c:2: 'default' label not within a switch statement
EOF
test_compile_error <<'EOF'
int main() {
    default:
        return 0;
}
EOF
