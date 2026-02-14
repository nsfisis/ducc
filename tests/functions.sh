# function pointers
cat <<'EOF' > expected
a
h
g
EOF

test_diff <<'EOF'
int* f1(int a);
int (*f2)(int a);

extern int atexit (void (*) (void));
extern int atexit (void (*fn) (void));

int printf(const char*, ...);

void g() { printf("g\n"); }
void h() { printf("h\n"); }

int main() {
    atexit(g);
    atexit(h);
    printf("a\n");
}
EOF

# void functions
cat <<'EOF' > expected
123
EOF
test_diff <<'EOF'
int printf();

void foo_bar(int hoge_piyo) {
    printf("%d\n", hoge_piyo);
}

int main() {
    foo_bar(123);
    return 0;
}
EOF

cat <<'EOF' > expected
EOF
test_diff <<'EOF'
struct S {
    int a;
};

struct S* f();

struct S* g() {}

int main() {
    return 0;
}
EOF

cat <<'EOF' > expected
hi
EOF
test_diff <<'EOF'
int printf();

void f() {
    printf("hi\n");
    return;
}

int main() {
    f();
    return 0;
}
EOF

# variadic functions
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

# implicit return
# C99: 5.1.2.2.3
test_exit_code 0 <<'EOF'
int main() {
}
EOF

test_exit_code 0 <<'EOF'
int main() {
    1 + 2 + 3;
}
EOF

test_exit_code 0 <<'EOF'
int main() {
    if (1);
    else return 1;
}
EOF
