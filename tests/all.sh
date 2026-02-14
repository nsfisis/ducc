set -e

rm -rf tests/tmp
mkdir -p tests/tmp

for filename in tests/*.sh tests/*.c; do
    testcase="$(basename "$filename")"
    case "$testcase" in
        all.sh|run.sh|helpers.sh|helpers.h)
            ;;
        *)
            source tests/run.sh "$testcase"
            ;;
    esac
done

echo "All tests passed."
