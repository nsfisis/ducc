cat <<'EOF' > expected
10 20
100 300 500
EOF

test_diff <<'EOF'
int printf();
in\
t ma\
in() {
    int x = 1\
0;

    int y = 2\
\
\
0;

#def\
ine X 100
#define Y 100 + \
200
#define Z /*
*/ 500
    // comment \
    x *= 100, y *= 200;
    /* comment *\
/

    printf("%d %d\n", x, y);
    printf("%d %d %d\n", X, Y, Z);
}
EOF

cat <<'EOF' > expected
main.c:1: <new-line> expected, but got <eof>
EOF

echo -n 'int main() {}\' | test_compile_error

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

