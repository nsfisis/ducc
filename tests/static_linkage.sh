cat > main.c <<'EOF'
int f1() { return 0; }
int* f2() { return 0; }
int** f3() { return 0; }

static int f4() { return 0; }
static int* f5() { return 0; }
static int** f6() { return 0; }

char* f7() { return 0; }
static char* f8() { return 0; }

void** f9() { return 0; }
static void** f10() { return 0; }

int main() { }
EOF

"$ducc" -o main.s main.c

function assert_global() {
    local func=$1
    if ! grep -q "\.globl $func\$" main.s; then
        echo "expected .globl for non-static function: $func" >&2
        exit 1
    fi
}

function assert_local() {
    local func=$1
    if grep -q "\.globl $func\$" main.s; then
        echo "unexpected .globl for static function: $func" >&2
        exit 1
    fi
}

assert_global f1
assert_global f2
assert_global f3
assert_global f7
assert_global f9
assert_global main

assert_local f4
assert_local f5
assert_local f6
assert_local f8
assert_local f10
