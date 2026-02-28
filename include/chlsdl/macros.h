#ifndef MACROS_H_
#define MACROS_H_

#include <stdlib.h>

#define STRINGIFY(x)            (#x)
#define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)
#undef STRINGIFY

#define CONCAT(a, b)    a##b
#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))

#if __STDC_VERSION__ >= 202311L
#    define CHLSDL_NORETURN      [[noreturn]]
#    define CHLSDL_MAYBE_UNUSED  [[maybe_unused]]
#    define CHLSDL_ALWAYS_INLINE [[gnu::always_inline]]
#    define __chlsdl_defer(f)    [[gnu::cleanup(f)]]
#else
#    define CHLSDL_NORETURN      __attribute__((noreturn))
#    define CHLSDL_MAYBE_UNUSED  __attribute__((unused))
#    define CHLSDL_ALWAYS_INLINE __attribute__((always_inline))
#    define __chlsdl_defer(f)    __attribute__((cleanup(f)))
#endif

#define chlsdl_defer __chlsdl_defer(__chlsdl_defer_free)

CHLSDL_ALWAYS_INLINE inline void
__chlsdl_defer_free(void * pp)
{
    free(*(void **)pp);
}

#define KILOBYTES_TO_BYTES(kb) (kb * 1024)
#define MEGABYTES_TO_BYTES(mb) (KILOBYTES(KILOBYTES(mb)))
#define MS_TO_NS(ms)           (ms * 1000000)

#define lambda(body) (body)

#endif // MACROS_H_
