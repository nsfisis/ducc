set -e

rm -rf tests/tmp
mkdir -p tests/tmp
for i in $(seq 1 999); do
    testcase=$(printf '%03d' $i)
    test_file="tests/$testcase.sh"
    if [[ -f "$test_file" ]]; then
        bash tests/run.sh "$testcase"
    else
        echo "All tests passed."
        exit
    fi
done
