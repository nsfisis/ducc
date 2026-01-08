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
