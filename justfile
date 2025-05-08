CFLAGS := "-Wno-builtin-declaration-mismatch"

all: build

build N="1":
    #!/usr/bin/env bash
    if [[ {{N}} = 1 ]]; then
        gcc -g -O0 -o ducc main.c {{CFLAGS}}
    else
        if [[ {{N}} = 2 ]]; then
            prev=""
        else
            prev=$(({{N}} - 1))
        fi
        "./ducc${prev}" < main.c > main{{N}}.s
        gcc -s -Wl,-z,noexecstack -o ducc{{N}} main{{N}}.s
    fi

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
