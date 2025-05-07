cat > main.c

"$p4dcc" < main.c > main.s
if [[ $? -ne 0 ]]; then
    cat main.s >&2
    exit 1
fi
gcc -Wl,-z,noexecstack -o a.out main.s
output="$(./a.out)"
exit_code=$?

if [[ $exit_code -ne 0 ]]; then
    echo "invalid exit code: $exit_code" >&2
    exit 1
fi

expected="$1"

if [[ "$output" != "$expected" ]]; then
    echo "invalid output: expected '$expected', but got '$output'" >&2
    exit 1
fi
