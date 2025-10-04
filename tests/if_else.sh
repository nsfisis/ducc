test_exit_code 0 <<'EOF'
#include "../../helpers.h"

int main() {
    int result1;
    if (1) {
        result1 = 12;
    } else {
        result1 = 34;
    }
    ASSERT_EQ(12, result1);

    int result2;
    if (1 + 1 != 2) {
        result2 = 12;
    } else {
        result2 = 34;
    }
    ASSERT_EQ(34, result2);
}
EOF
