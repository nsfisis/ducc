set -e

cat <<'EOF' > expected
Hello, World!
EOF

bash ../../test_example.sh hello
