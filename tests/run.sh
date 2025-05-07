set -e

export p4dcc="../../../$BIN"

export testcase=$1
export tmp_dir="tests/tmp/$testcase"

test_file="tests/$testcase.sh"

if [[ ! -f "$test_file" ]]; then
    echo "no test $testcase" >&2
    exit 1
fi

echo "$test_file"
mkdir -p "$tmp_dir"
cd "$tmp_dir"
bash "../../../$test_file"
