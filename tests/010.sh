test_exit_code 12 <<'EOF'
int main() {
    if (1) {
        return 12;
    } else {
        return 34;
    }
}
EOF

test_exit_code 34 <<'EOF'
int main() {
    if (1 + 1 != 2) {
        return 12;
    } else {
        return 34;
    }
}
EOF
