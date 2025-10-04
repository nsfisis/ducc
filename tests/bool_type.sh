test_exit_code 0 <<'EOF'
#include "../../helpers.h"

int main() {
    bool b1 = true, b0 = false;
    ASSERT_EQ(1, b1);
    ASSERT_EQ(0, b0);
    ASSERT_EQ(1, sizeof(b1));
    ASSERT_EQ(1, sizeof(b0));
    ASSERT_EQ(1, sizeof(bool));
}
EOF
