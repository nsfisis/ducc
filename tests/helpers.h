#ifndef DUCC_TESTS_HELPERS_H
#define DUCC_TESTS_HELPERS_H

void exit(int);
struct FILE;
typedef struct FILE FILE;
int fprintf(FILE*, const char*, ...);
extern FILE* stderr;

#define ASSERT(a, file, line) \
    do { \
        if (!(a)) { \
            fprintf(stderr, "%s:%d: assert failed", file, line); \
            exit(1); \
        } \
    } while (0)

#define ASSERT_EQ(a, b) ASSERT((a) == (b), __FILE__, __LINE__)

#endif
