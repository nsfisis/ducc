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
