cat > main.c

"$ducc" main.c > main.s
if [[ $? -ne 0 ]]; then
    exit 1
fi
gcc -o a.out main.s
./a.out
exit_code=$?

expected=$1

if [[ $exit_code -ne $expected ]]; then
    echo "invalid exit code: expected $expected, but got $exit_code" >&2
    exit 1
fi
