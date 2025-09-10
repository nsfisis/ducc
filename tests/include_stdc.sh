cat <<'EOF' > expected
EOF

test_diff <<'EOF'
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include <time.h>
#include <uchar.h>

int main() {}
EOF
