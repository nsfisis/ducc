function test_exit_code() {
    cat > main.c

    "$ducc" "${CFLAGS:-}" -o a.out main.c
    set +e
    ./a.out
    exit_code=$?
    set -e

    expected=$1

    if [[ $exit_code -ne $expected ]]; then
        echo "invalid exit code: expected $expected, but got $exit_code" >&2
        exit 1
    fi
}

function test_diff() {
    cat > main.c

    "$ducc" "${CFLAGS:-}" -o a.out main.c
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
}

function test_compile_error() {
    cat > main.c

    set +e
    "$ducc" "${CFLAGS:-}" main.c > /dev/null 2> output
    exit_code=$?
    set -e

    if [[ $exit_code -eq 0 ]]; then
        echo "expected to fail" >&2
        exit 1
    fi

    diff -u expected output
}

function test_cpp() {
    cat > main.c

    "$ducc" "${CFLAGS:-}" -E main.c > output
    diff -u -Z expected output
}

function test_example() {
    filename="../../../examples/$1.c"

    "$ducc" "${CFLAGS:-}" -o a.out "$filename"
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
}
