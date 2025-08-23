set -e

cat <<'EOF' > expected
Hello, World!
EOF

test_example hello
