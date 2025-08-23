cat <<'EOF' > expected
39
34
63
92
7
8
12
10
13
9
11
EOF
test_diff <<'EOF'
int printf();

int main() {
    printf("%d\n", '\'');
    printf("%d\n", '\"');
    printf("%d\n", '\?');
    printf("%d\n", '\\');
    printf("%d\n", '\a');
    printf("%d\n", '\b');
    printf("%d\n", '\f');
    printf("%d\n", '\n');
    printf("%d\n", '\r');
    printf("%d\n", '\t');
    printf("%d\n", '\v');
    return 0;
}
EOF
