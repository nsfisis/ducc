set -e

cat <<'EOF' > expected
hello, world
EOF

cat <<'EOF' > main.c
int printf();
int main() {
    printf("hello, world\n");
    return 0;
}
EOF

"$ducc" - < main.c > main.s
if [[ $? -ne 0 ]]; then
    cat main.s >&2
    exit 1
fi
gcc -o a.out main.s
./a.out "$@" > output
exit_code=$?

if [[ $exit_code -ne 0 ]]; then
    echo "invalid exit code: $exit_code" >&2
    exit 1
fi

diff -u expected output
