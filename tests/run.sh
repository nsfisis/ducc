set -e

export ducc="../../../build/$BIN"

arg=$1
export testcase="${arg%.sh}"
testcase="${testcase%.c}"
export tmp_dir="tests/tmp/$testcase"

run_c=0
run_sh=0
if [[ "$arg" == *.c ]]; then
    run_c=1
elif [[ "$arg" == *.sh ]]; then
    run_sh=1
else
    run_c=1
    run_sh=1
fi

found=0

if [[ $run_c -eq 1 && -f "tests/$testcase.c" ]]; then
    found=1
    source tests/helpers.sh

    echo "tests/$testcase.c"
    mkdir -p "$tmp_dir"
    cd "$tmp_dir"
    test_exit_code 0 < "../../../tests/$testcase.c"
    cd "../../.."
fi

if [[ $run_sh -eq 1 && -f "tests/$testcase.sh" ]]; then
    found=1
    source tests/helpers.sh

    echo "tests/$testcase.sh"
    mkdir -p "$tmp_dir"
    cd "$tmp_dir"
    source "../../../tests/$testcase.sh"
    cd "../../.."
fi

if [[ $found -eq 0 ]]; then
    echo "no test $testcase" >&2
    exit 1
fi
