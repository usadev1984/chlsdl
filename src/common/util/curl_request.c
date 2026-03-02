#include <chlsdl/common/print.h>
#include <chlsdl/common/util/curl_request.h>
#include <stdlib.h>
#include <string.h>

static const char * logfile_path;

struct curl_buffer *
curl_buffer_alloc(size_t n)
{
    struct curl_buffer * buf = malloc(sizeof(*buf));
    if (!buf)
        return NULL;

    buf->data = malloc(sizeof(*buf->data) + n);
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    buf->at   = 0;
    buf->size = n;
    return buf;
}

void
unset_curl_logfile_path()
{
    free((char *)logfile_path);
}

void
set_curl_logfile_path(const char * path)
{
    if (logfile_path)
        unset_curl_logfile_path();

    logfile_path = strdup(path);
    print_debug_warn("curl log file: '%s'\n", logfile_path);
}
