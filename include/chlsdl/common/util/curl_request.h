#ifndef CURL_REQUEST_H_
#define CURL_REQUEST_H_

#include <chlsdl/macros.h>
#include <stdlib.h>
#include <string.h>

struct curl_buffer {
    size_t size;
    size_t at;
    char * data;
};

extern struct curl_buffer *
curl_buffer_alloc(size_t n);

CHLSDL_ALWAYS_INLINE void
curl_buffer_dealloc(struct curl_buffer * buf)
{
    if (!buf)
        return;

    if (buf->data)
        free(buf->data);

    free(buf);
}

#endif // CURL_REQUEST_H_
