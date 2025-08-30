cat <<'EOF' > expected
EOF

test_diff <<'EOF'
#if 1 // a
#else // b
#endif // c

int main() {}
EOF
