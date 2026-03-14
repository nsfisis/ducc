#include <helpers.h>

static_assert(1);
static_assert(1, "always true");
static_assert(1 + 1);
static_assert(123 == 123);
static_assert(sizeof(int) == sizeof(unsigned int));

int main() {
}
