#include <chlsdl/common/common.h>

const struct version *
get_libchlsdl_common_version()
{
    static const struct version
        = { CHLSDL_VERSION, CHLSDL_MAJOR, CHLSDL_MINOR, CHLSDL_PATCH };
    return &version;
}
