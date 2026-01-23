cat <<'EOF' > header.h
int add(int a, int b) {
    return a + b;
}

int printf(const char*, ...);
EOF

cat <<'EOF' > expected
8
EOF
test_diff <<'EOF'
#include "header.h"

int main() {
    printf("%d\n", add(5, 3));
    return 0;
}
EOF

mkdir -p foo
cat <<'EOF' > bar.h
#include "baz.h"
EOF
cat <<'EOF' > baz.h
#define A 123
EOF
cat <<'EOF' > foo/bar.h
#include "baz.h"
EOF
cat <<'EOF' > foo/baz.h
#define A 456
EOF

cat <<'EOF' > expected
123
456
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
#include "bar.h"
    printf("%d\n", A);

#undef A

#include "foo/bar.h"
    printf("%d\n", A);
}
EOF

cat <<'EOF' > expected
12
EOF

cat <<'EOF' > math.h
int multiply(int a, int b) {
    return a * b;
}
EOF

cat <<'EOF' > calc.h
#include "math.h"

int calculate(int x) {
    return multiply(x, 2);
}

int printf(const char*, ...);
EOF

test_diff <<'EOF'
#include "calc.h"

int main() {
    printf("%d\n", calculate(6));
    return 0;
}
EOF

cat <<'EOF' > expected
42
EOF

cat <<'EOF' > header.h
#define A 42
EOF

test_diff <<'EOF'
#include "header.h"

int printf();

int main() {
    printf("%d\n", A);
    return 0;
}
EOF

cat <<'EOF' > expected
42
EOF

cat <<'EOF' > header.h
#ifndef HEADER_H
#define HEADER_H

int f() { return 42; }

#endif
EOF

test_diff <<'EOF'
#include "header.h"
#include "header.h"
#include "header.h"

int printf();

int main() {
    printf("%d\n", HEADER_H HEADER_H HEADER_H f() HEADER_H HEADER_H HEADER_H);
    return 0;
}
EOF

cat <<'EOF' > expected
main.c:1: cannot open include file: "nonexistent.h"
EOF

test_compile_error <<'EOF'
#include "nonexistent.h"

int main() {
    return 0;
}
EOF

cat <<'EOF' > expected
include depth limit exceeded
EOF

# Create circular include files
cat <<'EOF' > a.h
#include "b.h"
int a() { return 1; }
EOF

cat <<'EOF' > b.h
#include "a.h"
int b() { return 2; }
EOF

test_compile_error <<'EOF'
#include "a.h"

int main() {
    a() + b();
    return 0;
}
EOF

cat <<'EOF' > expected
main.c:1: cannot open include file: "hoge.h"
EOF

test_compile_error <<'EOF'
#include "hoge.h"
EOF

cat <<'EOF' > expected
main.c:1: cannot resolve include file name: <hoge.h>
EOF

test_compile_error <<'EOF'
#include <hoge.h>
EOF

cat <<'EOF' > expected
42
EOF

test_exit_code 0 <<'EOF'
#ifdef FOO
#include FOO
#endif

int main() { 1 < 2; }
EOF
