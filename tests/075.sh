set -e

cat <<'EOF' > expected
EOF

bash ../../test_diff.sh <<'EOF'
/* lorem ipsum */
/*
lorem ipsum
*/
// /*
/* /*
 */
int/**/main() {
}
EOF
