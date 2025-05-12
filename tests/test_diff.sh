cat > main.c

"$ducc" main.c > main.s
if [[ $? -ne 0 ]]; then
    exit 1
fi
gcc -Wl,-z,noexecstack -o a.out main.s
if [[ ! -f input ]]; then
    touch input
fi
./a.out "$@" < input > output
exit_code=$?

if [[ $exit_code -ne 0 ]]; then
    echo "invalid exit code: $exit_code" >&2
    exit 1
fi

diff -u expected output
