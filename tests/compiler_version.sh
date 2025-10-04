expected="ducc v0.2.0"

if [[ "$("$ducc" --version)" != "$expected" ]]; then
    echo "invalid output" >&2
    exit 1
fi

