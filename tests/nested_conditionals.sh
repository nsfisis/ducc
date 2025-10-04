cat <<'EOF' > expected
EOF
test_diff <<'EOF'
#if 0
# if 1
#  error "not an error"
# endif
#endif

int main() {}
EOF

