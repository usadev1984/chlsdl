#ifndef SOCKET_H_
#define SOCKET_H_

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

#endif // SOCKET_H_
