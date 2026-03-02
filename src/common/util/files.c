#include <chlsdl/common/print.h>
#include <chlsdl/common/util/curl_request.h>
#include <chlsdl/common/util/files.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
write_buffer_to_file(const char * f, size_t sz, const void * data)
{
    print_debug_warn("writing to: '%s'\n", f);
    FILE * fp = fopen(f, "w");
    assert(fp);

    fwrite(data, sizeof(char), sz > 0 ? sz : strlen(data), fp);

    fclose(fp);
}

struct curl_buffer *
read_file_to_buffer(const char * f)
{
    FILE * fp = fopen(f, "r");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    struct curl_buffer * buf = curl_buffer_alloc(len);
    assert(buf);
    buf->at = buf->size = len;

    fread(buf->data, sizeof(*buf->data), len, fp);

    fclose(fp);

    return buf;
}
