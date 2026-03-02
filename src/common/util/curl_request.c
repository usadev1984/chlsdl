#include <chlsdl/common/util/curl_request.h>
#include <stdlib.h>

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
