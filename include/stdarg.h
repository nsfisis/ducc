#ifndef __DUCC_BUILTIN___STDARG_H__
#define __DUCC_BUILTIN___STDARG_H__

// System V Application Binary Interface
// AMD64 Architecture Processor Supplement
// (With LP64 and ILP32 Programming Models)
// Version 1.0
// Figure 3.34: va_list Type Declaration
struct __ducc_va_list {
    // unsigned int gp_offset;
    // unsigned int fp_offset;
    int gp_offset;
    int fp_offset;
    void* overflow_arg_area;
    void* reg_save_area;
};
// ducc currently does not support array type.
// typedef struct __ducc_va_list va_list[1];
typedef struct __ducc_va_list* va_list;

#define va_start(args, start) __ducc_va_start(args, start)
#define va_end(args)

#endif
