test_exit_code 0 <<'EOF'
#include "../../helpers.h"

int main() {
    ASSERT_EQ(2, 1 ? 2 : 3);
    ASSERT_EQ(5, 0 ? 4 : 5);
}
EOF
