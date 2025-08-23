cat <<'EOF' > expected
cannot open include file: nonexistent.h
EOF

test_compile_error <<'EOF'
#include "nonexistent.h"

int main() {
    return 0;
}
EOF
