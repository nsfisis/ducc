# C99: 5.1.2.2.3
test_exit_code 0 <<'EOF'
int main() {
}
EOF

test_exit_code 0 <<'EOF'
int main() {
    1 + 2 + 3;
}
EOF

test_exit_code 0 <<'EOF'
int main() {
    if (1);
    else return 1;
}
EOF

