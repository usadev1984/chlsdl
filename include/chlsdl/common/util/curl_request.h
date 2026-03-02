#ifndef CURL_REQUEST_H_
#define CURL_REQUEST_H_

#include <string.h>

struct curl_buffer {
    size_t size;
    size_t at;
    char * data;
};

extern struct curl_buffer *
curl_buffer_alloc(size_t n);

#endif // CURL_REQUEST_H_
