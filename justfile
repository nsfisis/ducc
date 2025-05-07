CFLAGS := "-Wno-builtin-declaration-mismatch"

all: build

build N="1":
    #!/usr/bin/env bash
    if [[ {{N}} = 1 ]]; then
        gcc -g -O0 -o p4dcc main.c {{CFLAGS}}
    else
        if [[ {{N}} = 2 ]]; then
            prev=""
        else
            prev=$(({{N}} - 1))
        fi
        "./p4dcc${prev}" < main.c > main{{N}}.s
        gcc -s -Wl,-z,noexecstack -o p4dcc{{N}} main{{N}}.s
    fi

build-upto-5-gen:
    just build 1
    just build 2
    just build 3
    just build 4
    just build 5

test-self-hosted: build-upto-5-gen
    diff -u ./p4dcc2 ./p4dcc3
    diff -u ./p4dcc3 ./p4dcc4
    diff -u ./p4dcc4 ./p4dcc5

test TESTCASE="all" $BIN="p4dcc": build
    #!/usr/bin/env bash
    if [[ {{TESTCASE}} = all ]]; then
        bash tests/all.sh
    else
        bash tests/run.sh {{TESTCASE}}
    fi

test-all:
    just test-self-hosted
    just test all p4dcc
    just test all p4dcc2
    just test all p4dcc3
    just test all p4dcc4
    just test all p4dcc5

clean:
    rm -f main*.s
    rm -f p4dcc*
    rm -rf tests/tmp
