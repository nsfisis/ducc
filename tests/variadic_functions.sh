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

