set -e

rm -rf tests/tmp
mkdir -p tests/tmp

for filename in tests/*.sh; do
    testcase_="$(basename "$filename")"
    testcase="${testcase_/%.sh/}"
    test_file="tests/$testcase.sh"
    case "$testcase" in
        all|run|test_*)
            ;;
        *)
            bash tests/run.sh "$testcase"
            ;;
    esac
done

echo "All tests passed."
