test_exit_code 42 <<'EOF'
int main() {
    return 42;
}
EOF

test_exit_code 21 <<'EOF'
int main() {
    return 5+20-4;
}
EOF

test_exit_code 26 <<'EOF'
int main() {
    return 2*3+4*5;
}
EOF

test_exit_code 197 <<'EOF'
int main() {
    return (((3+5)/2) + (5*(9-6)) * (5+6*7)) % 256;
}
EOF

test_exit_code 30 <<'EOF'
int main() {
    return (-10 + 20 * -3) + 100;
}
EOF

