cat <<'EOF' > expected
EOF

test_diff <<'EOF'
int main() { ; }
EOF
