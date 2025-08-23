cat <<'EOF' > expected
EOF

test_diff <<'EOF'
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
