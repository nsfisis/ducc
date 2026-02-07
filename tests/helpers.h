#ifndef DUCC_TESTS_HELPERS_H
#define DUCC_TESTS_HELPERS_H

void exit(int);
struct FILE;
typedef struct FILE FILE;
int fprintf(FILE*, const char*, ...);
extern FILE* stderr;
int strcmp(const char*, const char*);

#define ASSERT(a, file, line) \
    do { \
        if (!(a)) { \
            fprintf(stderr, "%s:%d: assert failed\n", file, line); \
            exit(1); \
        } \
    } while (0)

#define ASSERT_EQ(a, b) ASSERT((a) == (b), __FILE__, __LINE__)
#define ASSERT_EQ_STR(a, b) ASSERT(strcmp((a), (b)) == 0, __FILE__, __LINE__)

#endif
