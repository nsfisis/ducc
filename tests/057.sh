set -e

cat <<'EOF' > expected
cannot open include file: nonexistent.h
EOF

bash ../../test_compile_error.sh <<'EOF'
#include "nonexistent.h"

int main() {
    return 0;
}
EOF
