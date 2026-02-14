#include <helpers.h>

int main() {
    int i;
    int ret;
    i = 0;
    ret = 0;
    for (i = 0; i < 10; i = i + 1) {
        ret = ret + i;
    }
    ASSERT_EQ(45, ret);

    i = 0;
    ret = 0;
    for (i = 0; i < 10; i = i + 1) {
        if (i % 2 == 0) {
            continue;
        }
        ret = ret + i;
    }
    ASSERT_EQ(25, ret);

    i = 0;
    ret = 0;
    for (i = 0; i < 100; i = i + 1) {
        if (i == 12) {
            break;
        }
        ret = ret + i;
    }
    ASSERT_EQ(66, ret);

    int sum = 0;
    i = 0;
    for (; i < 5; i = i + 1) {
        sum = sum + i;
    }
    ASSERT_EQ(10, sum);

    sum = 0;
    for (i = 10; i < 15; ) {
        sum = sum + i;
        i = i + 1;
    }
    ASSERT_EQ(60, sum);

    sum = 0;
    for (i = 20; ; i = i + 1) {
        sum = sum + i;
        if (i == 25) break;
    }
    ASSERT_EQ(135, sum);

    sum = 0;
    for (int j = 0; j < 10; j++) {
        sum = sum + j;
    }
    ASSERT_EQ(45, sum);

    int sum1 = 0;
    for (int j = 0; j < 5; j++) {
        sum1 = sum1 + j;
    }
    int sum2 = 0;
    for (int j = 0; j < 5; j++) {
        sum2 = sum2 + j;
    }
    ASSERT_EQ(10, sum1);
    ASSERT_EQ(10, sum2);

    int x = 42;
    {
        int x = 43;
        ASSERT_EQ(43, x);
    }
    ASSERT_EQ(42, x);

    int last_i = -1;
    int last_j = -1;
    for (int k = 0, l = 1; k < 5; k++, l += 2) {
        last_i = k;
        last_j = l;
    }
    ASSERT_EQ(4, last_i);
    ASSERT_EQ(9, last_j);
}
