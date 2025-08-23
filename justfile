all: build

build N="1":
    #!/usr/bin/env bash
    set -e
    if [[ {{N}} = 1 ]]; then
        cc=gcc
        target=ducc
    elif [[ {{N}} = 2 ]]; then
        cc=./ducc
        target=ducc{{N}}
    else
        cc="./ducc$(({{N}} - 1))"
        target=ducc{{N}}
    fi
    "$cc" -g -O0 -o "$target" main.c

build-upto-5-gen:
    just build 1
    just build 2
    just build 3
    just build 4
    just build 5

test-self-hosted: build-upto-5-gen
    diff -u ./ducc2 ./ducc3
    diff -u ./ducc3 ./ducc4
    diff -u ./ducc4 ./ducc5

test TESTCASE="all" $BIN="ducc": build
    #!/usr/bin/env bash
    set -e
    if [[ {{TESTCASE}} = all ]]; then
        bash tests/all.sh
    else
        bash tests/run.sh {{TESTCASE}}
    fi

test-all:
    just test-self-hosted
    just test all ducc
    just test all ducc2

clean:
    rm -f main*.s
    rm -f ducc*
    rm -rf tests/tmp
