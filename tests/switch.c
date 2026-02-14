#include <helpers.h>

int switch_fallthrough(int x) {
    switch (x) {
    case 1:
    case 2:
        return 12;
    case 3:
    case 4:
        return 34;
    }
    return 0;
}

int main() {
    int x = 2;
    int result = 0;

    switch (x) {
    case 1:
        result = 10;
        break;
    case 2:
        result = 20;
        break;
    case 3:
        result = 30;
        break;
    }
    ASSERT_EQ(20, result);

    x = 5;
    result = 0;
    switch (x) {
    case 1:
        result = 10;
        break;
    case 2:
        result = 20;
        break;
    default:
        result = 99;
        break;
    }
    ASSERT_EQ(99, result);

    x = 2;
    result = 0;
    switch (x) {
    case 1:
        result = result + 10;
    case 2:
        result = result + 20;
    case 3:
        result = result + 30;
        break;
    }
    ASSERT_EQ(50, result);

    x = 1;
    int y = 2;
    result = 0;
    switch (x) {
    case 1:
        switch (y) {
        case 1:
            result = 11;
            break;
        case 2:
            result = 12;
            break;
        }
        break;
    case 2:
        result = 20;
        break;
    }
    ASSERT_EQ(12, result);

    int a = 3;
    int b = 2;
    result = 0;
    switch (a + b) {
    case 4:
        result = 40;
        break;
    case 5:
        result = 50;
        break;
    case 6:
        result = 60;
        break;
    }
    ASSERT_EQ(50, result);

    x = 2;
    result = 0;
    int temp = 0;
    switch (x) {
    case 1:
        temp = 5;
        result = temp * 2;
        break;
    case 2:
        temp = 10;
        result = temp * 2;
        break;
    case 3:
        temp = 15;
        result = temp * 2;
        break;
    }
    ASSERT_EQ(20, result);
    ASSERT_EQ(10, temp);

    x = 1;
    result = 0;
    switch (x) {
    case 1: {
        int local = 100;
        result = local;
        break;
    }
    case 2: {
        int local = 200;
        result = local;
        break;
    }
    }
    ASSERT_EQ(100, result);

    x = 10;
    result = 42;
    switch (x) {
    case 1:
        result = 10;
        break;
    case 2:
        result = 20;
        break;
    }
    ASSERT_EQ(42, result);

    ASSERT_EQ(12, switch_fallthrough(1));
    ASSERT_EQ(12, switch_fallthrough(2));
    ASSERT_EQ(34, switch_fallthrough(3));
    ASSERT_EQ(34, switch_fallthrough(4));
}
