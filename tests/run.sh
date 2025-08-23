set -e

export ducc="../../../$BIN"

export testcase=$1
export tmp_dir="tests/tmp/$testcase"

test_file="tests/$testcase.sh"

if [[ ! -f "$test_file" ]]; then
    echo "no test $testcase" >&2
    exit 1
fi

source tests/test_helpers.sh

echo "$test_file"
mkdir -p "$tmp_dir"
cd "$tmp_dir"
source "../../../$test_file"
cd "../../.."
