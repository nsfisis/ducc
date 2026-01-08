set -e

export ducc="../../../build/$BIN"

export testcase=$1
export tmp_dir="tests/tmp/$testcase"

c_test_file="tests/$testcase.c"
sh_test_file="tests/$testcase.sh"

if [[ -f "$c_test_file" ]]; then
    source tests/helpers.sh

    echo "$c_test_file"
    mkdir -p "$tmp_dir"
    cd "$tmp_dir"
    test_exit_code 0 < "../../../$c_test_file"
    cd "../../.."
elif [[ -f "$sh_test_file" ]]; then
    source tests/helpers.sh

    echo "$sh_test_file"
    mkdir -p "$tmp_dir"
    cd "$tmp_dir"
    source "../../../$sh_test_file"
    cd "../../.."
else
    echo "no test $testcase" >&2
    exit 1
fi
