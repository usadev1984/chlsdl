#ifndef SOCKET_H_
#define SOCKET_H_

#include <chlsdl-modules/chlsdl-common/util/curl_request.h>
#include <chlsdl/macros.h>
#include <stdint.h>
#include <unistd.h>

extern int
socket_open(uint16_t port);

CHLSDL_ALWAYS_INLINE inline void
socket_close(int sockfd)
{
    close(sockfd);
}

extern ssize_t
socket_recv_no_http_header(int sockfd, struct curl_buffer * buf);

#endif // SOCKET_H_
