build N="1":
    #!/usr/bin/env bash
    set -e
    if [[ {{N}} = 1 ]]; then
        cc=gcc
        target=ducc
    elif [[ {{N}} = 2 ]]; then
        cc=./build/ducc
        target=ducc{{N}}
    else
        cc="./build/ducc$(({{N}} - 1))"
        target=ducc{{N}}
    fi
    CC="$cc" TARGET="$target" make

build-upto-5-gen:
    just build 1
    just build 2
    just build 3
    just build 4
    just build 5

test-self-hosted: build-upto-5-gen
    diff -u ./build/ducc2 ./build/ducc3
    diff -u ./build/ducc3 ./build/ducc4
    diff -u ./build/ducc4 ./build/ducc5

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
