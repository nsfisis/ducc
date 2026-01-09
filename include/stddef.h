#ifndef __DUCC_BUILTIN___STDDEF_H__
#define __DUCC_BUILTIN___STDDEF_H__

#define NULL 0

typedef unsigned long size_t;
typedef int wchar_t;
typedef long ptrdiff_t;

#define offsetof(T, m) (ptrdiff_t)((void*)((T*)0)->m)

// TODO:
// - max_align_t (C11)
// - nullptr_t (C23)

#endif
