function test_exit_code() {
    set +e
    cat > main.c

    "$ducc" -o a.out main.c
    ./a.out
    exit_code=$?

    expected=$1

    if [[ $exit_code -ne $expected ]]; then
        echo "invalid exit code: expected $expected, but got $exit_code" >&2
        set -e
        exit 1
    fi
    set -e
}

function test_diff() {
    cat > main.c

    "$ducc" -o a.out main.c
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
    set +e
    cat > main.c

    "$ducc" main.c > /dev/null 2> output
    if [[ $? -eq 0 ]]; then
        "expected to fail"
        set -e
        exit 1
    fi

    diff -u expected output
    set -e
}

function test_example() {
    filename="../../../examples/$1.c"

    "$ducc" -o a.out "$filename"
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
