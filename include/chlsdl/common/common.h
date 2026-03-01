#ifndef COMMON_H_
#define COMMON_H_

struct version {
    const char * version;
    int          major;
    int          minor;
    int          patch;
};

extern const struct version *
get_libchlsdl_common_version();

#endif // COMMON_H_
