test_exit_code 1 <<'EOF'
int main() {
    return 0 == 0;
}
EOF

test_exit_code 0 <<'EOF'
int main() {
    return 123 != 123;
}
EOF

test_exit_code 1 <<'EOF'
int main() {
    return 123 != 456;
}
EOF

test_exit_code 0 <<'EOF'
int main() {
    return 123 == 124;
}
EOF

test_exit_code 1 <<'EOF'
int main() {
    return 123 < 567;
}
EOF

test_exit_code 1 <<'EOF'
int main() {
    return 123 <= 567;
}
EOF

test_exit_code 1 <<'EOF'
int main() {
    return 123 <= 123;
}
EOF

test_exit_code 0 <<'EOF'
int main() {
    return 123 < 123;
}
EOF
