cat <<'EOF' > expected
main.c:2: expected '#endif', but got '<eof>'
EOF
test_compile_error <<'EOF'
#if 0
EOF

cat <<'EOF' > expected
main.c:3: expected '#endif', but got '<eof>'
EOF
test_compile_error <<'EOF'
#if 1
#else
EOF

cat <<'EOF' > expected
main.c:3: expected '#endif', but got '<eof>'
EOF
test_compile_error <<'EOF'
#if 1
#elif 1
EOF
