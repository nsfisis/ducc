set -e

rm -rf tests/tmp
mkdir -p tests/tmp

for filename in tests/*.sh tests/*.c; do
    testcase="$(basename "$filename")"
    testcase="${testcase/%.sh/}"
    testcase="${testcase/%.c/}"
    case "$testcase" in
        all|run|helpers)
            ;;
        *)
            source tests/run.sh "$testcase"
            ;;
    esac
done

echo "All tests passed."
