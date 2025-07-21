set -e

cat <<'EOF' > expected
123
456 789
EOF

bash ../../test_diff.sh <<'EOF'
int fprintf();

// System V Application Binary Interface
// AMD64 Architecture Processor Supplement
// (With LP64 and ILP32 Programming Models)
// Version 1.0
// Figure 3.34: va_list Type Declaration
struct __va_list__ {
    // unsigned int gp_offset;
    // unsigned int fp_offset;
    int gp_offset;
    int fp_offset;
    void* overflow_arg_area;
    void* reg_save_area;
};
// ducc currently does not support array type.
// typedef struct __va_list__ va_list[1];
typedef struct __va_list__* va_list;

// va_start() is currently implemented as a special form due to the limitation of #define macro.
void va_end(va_list args) {
}

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
