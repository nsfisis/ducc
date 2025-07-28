set -e

cat <<'EOF' > expected
EOF

bash ../../test_diff.sh <<'EOF'
int main() { ; }
EOF
