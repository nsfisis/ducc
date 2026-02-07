# --version
expected="ducc v0.3.0"

if [[ "$("$ducc" --version)" != "$expected" ]]; then
    echo "invalid output" >&2
    exit 1
fi

# assembly output
cat > foo.c <<'EOF'
int main() {}
EOF

"$ducc" -o bar.s foo.c
if [[ $? -ne 0 ]]; then
    exit 1
fi
gcc -o a.out bar.s
./a.out "$@"
exit_code=$?

if [[ $exit_code -ne 0 ]]; then
    echo "invalid exit code: $exit_code" >&2
    exit 1
fi

# argc/argv
cat <<'EOF' > expected
argc = 4
argv[1] = hoge
argv[2] = piyo
argv[3] = fuga
EOF
test_diff hoge piyo fuga<<'EOF'
int printf();

int main(int argc, char** argv) {
    printf("argc = %d\n", argc);
    int i;
    for (i = 1; i < argc; ++i) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    return 0;
}
EOF

# compile errors
# TODO: report as "undefined function"
cat <<'EOF' > expected
main.c:2: undefined variable: f
EOF
test_compile_error <<'EOF'
int main() {
    f();
}
EOF

# TODO: improve error message
# cat <<'EOF' > expected
# main.c:1: expected ';' or '{', but got '}'
# EOF
cat <<'EOF' > expected
main.c:1: expected ';', but got '}'
EOF

test_compile_error <<'EOF'
int main() }
EOF

# TODO: improve error message
# cat <<'EOF' > expected
# main.c:1: expected ';' or '{', but got '}'
# EOF
cat <<'EOF' > expected
main.c:1: expected ';', but got '123'
EOF

test_compile_error <<'EOF'
int main() 123
EOF
