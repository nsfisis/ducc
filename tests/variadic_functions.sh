cat <<'EOF' > expected
123
456 789
EOF

test_diff <<'EOF'
#include <stdarg.h>

int fprintf();

struct FILE;
typedef struct FILE FILE;

extern FILE* stdout;

int vfprintf(FILE*, const char*, va_list);

void fatal_error(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stdout, msg, args);
    va_end(args);
    fprintf(stdout, "\n");
}

int main() {
    fatal_error("%d", 123);
    fatal_error("%d %d", 456, 789);
    return 0;
}
EOF

test_exit_code 0 <<'EOF'
#include <stdarg.h>
#include <helpers.h>

int sum(int n, ...) {
    va_list args;
    va_start(args, n);
    int s = 0;
    for (int i = 0; i < n; ++i) {
        s += va_arg(args, int);
    }
    va_end(args);
    return s;
}

int main() {
    ASSERT_EQ(400, sum(5, 100, 90, 80, 70, 60));
}
EOF
