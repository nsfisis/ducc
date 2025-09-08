cat <<'EOF' > expected
Hello World
Line continues
EOF

# CRLF
printf 'int printf(const char*, ...);\r\nint main() {\r\n    printf("Hello World\\n");\r\n    printf("Line con\\\r\ntinues\\n");\r\n    return 0;\r\n}\r\n' > main_crlf.c
test_diff < main_crlf.c

# CR
printf 'int printf(const char*, ...);\rint main() {\r    printf("Hello World\\n");\r    printf("Line con\\\rtinues\\n");\r    return 0;\r}\r' > main_cr.c
test_diff < main_cr.c

# Mixed
printf 'int printf(const char*, ...);\nint main() {\r\n    printf("Hello World\\n");\r    printf("Line con\\\r\ntinues\\n");\n    return 0;\r\n}\r\n' > main_mixed.c
test_diff < main_mixed.c
