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
