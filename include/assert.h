#ifndef __DUCC_BUILTIN___ASSERT_H__
#define __DUCC_BUILTIN___ASSERT_H__

#ifdef NDEBUG
#define assert(x) ((void)0)
#else
#define assert(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: assertion failed.\n", __FILE__, __LINE__); \
            abort(); \
        } \
    } while (0)
#endif

#endif
