#ifndef MODULE_H_
#define MODULE_H_

#include "common/common.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

struct chlsdl_data {
    const struct version * chlsdl_version;

    const char * cache_dir;
    const char * downloads_dir;
};

typedef const struct module * (*module_init)(const struct chlsdl_data *);
typedef void (*module_deinit)();

struct module {
    const module_deinit deinit;

    struct {
        pcre2_code *       pattern;
        pcre2_match_data * md;
    } regex;
};

#endif // MODULE_H_
