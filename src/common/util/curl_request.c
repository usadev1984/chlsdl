#include <chlsdl/common/print.h>
#include <chlsdl/common/util/curl_request.h>
#include <chlsdl/macros.h>
#include <curl/curl.h>
#include <stdio.h>
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

static size_t
curl_write_buffer(char * data, size_t s, size_t n, void * userdata)
{
    size_t rs = s * n;

    struct curl_buffer * m = userdata;

    m->data = realloc(m->data, m->size + rs);
    if (!m->data)
        return 0;
    memcpy(&(m->data[m->at]), data, rs);

    m->at += rs;
    m->size += rs;

    print_debug_warn("got '%lu' so far\n", m->size);
    return rs;
}

CHLSDL_ALWAYS_INLINE static inline void
curl_request_set_common_opts(CURL * curl, FILE ** curl_log)
{
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 0L);

    *curl_log = fopen(logfile_path, "w");
    if (!*curl_log) {
        print_debug_error("failed to open file\n");
        exit(1);
    }

    curl_easy_setopt(curl, CURLOPT_STDERR, *curl_log);
}

int
curl_request_get(const char * url, struct curl_buffer * buf)
{
    char curl_errbuf[CURL_ERROR_SIZE];

    CURL * curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_buffer);

    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);

    FILE * curl_log;
    curl_request_set_common_opts(curl, &curl_log);

    int err = curl_easy_perform(curl);

    fclose(curl_log);
    curl_easy_cleanup(curl);
    return err;
}

int
curl_request_post(const char * url, struct curl_buffer * buf,
    const char * postfields, curl_off_t postfieldsize_large)
{
    char curl_errbuf[CURL_ERROR_SIZE];

    CURL * curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    if (postfields) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);
        curl_easy_setopt(
            curl, CURLOPT_POSTFIELDSIZE_LARGE, postfieldsize_large);
    }

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_buffer);

    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);

    FILE * curl_log;
    curl_request_set_common_opts(curl, &curl_log);

    int err = curl_easy_perform(curl);

    fclose(curl_log);
    curl_easy_cleanup(curl);
    return err;
}
