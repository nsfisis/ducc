build N="1":
    #!/usr/bin/env bash
    set -e
    if [[ {{N}} = 1 ]]; then
        cc=gcc
        cflags=
        target=ducc
    elif [[ {{N}} = 2 ]]; then
        cc=./build/ducc
        cflags="${CFLAGS:-}"
        target=ducc{{N}}
    else
        cc="./build/ducc$(({{N}} - 1))"
        cflags="${CFLAGS:-}"
        target=ducc{{N}}
    fi
    CC="$cc" CFLAGS="$cflags" TARGET="$target" make

test-self-hosted: (build "1") (build "2") (build "3")
    diff -u ./build/ducc2 ./build/ducc3

test TESTCASE="all" $BIN="ducc": build
    #!/usr/bin/env bash
    set -e
    if [[ {{TESTCASE}} = all ]]; then
        bash tests/all.sh
    else
        bash tests/run.sh {{TESTCASE}}
    fi

test-all:
    just test all ducc
    just test-self-hosted
    just test all ducc2

clean:
    rm -rf build
    rm -rf tests/tmp
