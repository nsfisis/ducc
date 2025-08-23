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
