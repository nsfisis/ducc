test_exit_code 0 <<'EOF'
#include "../../helpers.h"

int fib(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}

int main() {
    ASSERT_EQ(89, fib(10));
}
EOF
