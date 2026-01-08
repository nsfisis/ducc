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
