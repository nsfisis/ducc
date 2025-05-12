cat > main.c

"$ducc" main.c > /dev/null 2> output
if [[ $? -eq 0 ]]; then
    "expected to fail"
    exit 1
fi

diff -u expected output
