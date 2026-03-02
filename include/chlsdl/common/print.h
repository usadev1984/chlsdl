#ifndef PRINT_H_
#define PRINT_H_

#include <stdio.h>

#ifdef COLOR
#    define COLOR_GREEN  "\033[38;2;0;255;0m"
#    define COLOR_RED    "\033[38;2;255;0;0m"
#    define COLOR_YELLOW "\033[38;2;255;255;0m"
#    define TERM_COLOR   "\033[00m"
#else
#    define COLOR_GREEN
#    define COLOR_RED
#    define COLOR_YELLOW
#    define TERM_COLOR
#endif

#define print_error(format, ...)                                               \
    fprintf(stderr, COLOR_RED format TERM_COLOR __VA_OPT__(, ) __VA_ARGS__)

#define print_warn(format, ...)                                                \
    fprintf(stdout, COLOR_YELLOW format TERM_COLOR __VA_OPT__(, ) __VA_ARGS__)

#define print_info(format, ...)                                                \
    fprintf(stdout, format __VA_OPT__(, ) __VA_ARGS__)

#ifdef DEBUG
#    define print_debug_error(format, ...)                                     \
        fprintf(stderr, COLOR_RED "%s:%d: " format TERM_COLOR, __func__,       \
            __LINE__ __VA_OPT__(, ) __VA_ARGS__)

#    define print_debug_warn(format, ...)                                      \
        fprintf(stderr, COLOR_YELLOW "%s:%d: " format TERM_COLOR, __func__,    \
            __LINE__ __VA_OPT__(, ) __VA_ARGS__)

#    define print_debug_success(format, ...)                                   \
        fprintf(stderr, COLOR_GREEN "%s:%d: " format TERM_COLOR, __func__,     \
            __LINE__ __VA_OPT__(, ) __VA_ARGS__)
#else
#    define print_debug_error(format, ...)   ({})
#    define print_debug_warn(format, ...)    ({})
#    define print_debug_success(format, ...) ({})
#endif

#endif // PRINT_H_
