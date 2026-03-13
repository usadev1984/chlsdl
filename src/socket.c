#include "socket.h"

#include <arpa/inet.h> // inet_addr()
#include <assert.h>
#include <chlsdl-modules/chlsdl-common/print.h>
#include <chlsdl-modules/chlsdl-common/util/curl_request.h>
#include <errno.h>
#include <stdint.h>
#include <sys/socket.h>

int
socket_open(uint16_t port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 6);
    if (sockfd == -1) {
        print_error("couldn't open socket\n");
        return -1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) { 1 }, sizeof(int))
        == -1) {
        socket_close(sockfd);
        return -1;
    }

    struct sockaddr_in server_address = {};

    // assign IP, PORT
    server_address.sin_family      = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port        = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address))
        == -1) {
        socket_close(sockfd);
        return -1;
    }

    if (listen(sockfd, 5) == -1) {
        socket_close(sockfd);
        return -1;
    }

    return sockfd;
}

static ssize_t
socket_recv(int sockfd, struct curl_buffer * buf)
{
    print_debug_warn("waiting for a connection...\n");
    int client_sockfd = accept(sockfd, NULL, NULL);
    if (client_sockfd == -1) {
        print_debug_error("%d: '%s'\n", errno, strerror(errno));
        return -1;
    }
    /* should we do more checking for security before proceeding? */

    buf->at = 0;
again:
    print_debug_warn("waiting for data...\n");
    ssize_t n
        = recv(client_sockfd, &buf->data[buf->at], buf->size, MSG_DONTWAIT);
    int errsv = errno;
    if (n == 0) {
        /* close(client_sockfd); */
        /* return 0; */
        assert(0); /* TODO: returning from here is problematic because `buf`
                    * might contain data from previous calls to `recv()`. idk do
                    * something about it later
                    */
    }

    if (n == -1) {
        switch (errsv) {
        case EAGAIN:
            print_debug_error("EAGAIN\n");
            goto again;
        default:
            print_debug_error("%d: '%s'\n", errsv, strerror(errsv));
        }
        print_error("couldn't read from socket\n");
        close(client_sockfd);
        return -1;
    }

    print_debug_success("received (%zu bytes): '%s'\n", n, &buf->data[buf->at]);

    /* data has been successfully read from the socket */
    buf->at += n;

    /* there may still be data available, resize and try again */
    if (n == buf->size) {
        const size_t new_size = buf->size * 2;
        void *       p        = realloc(buf->data, new_size + 1);
        assert(p);
        buf->data = p;
        buf->size = new_size;
        goto again;
    }

    close(client_sockfd);

    buf->data[buf->at] = '\0';

    return buf->at;
}

ssize_t
socket_recv_no_http_header(int sockfd, struct curl_buffer * buf)
{
    ssize_t r = socket_recv(sockfd, buf);
    if (r <= 0)
        return r;

    const char   needle[]   = "\r\n\r\n";
    const char * header_end = strstr(buf->data, needle);
    assert(header_end);
    header_end            = header_end + sizeof(needle) - 1;
    const size_t data_len = strlen(header_end);

    print_debug_warn("header_end: %zu\n", header_end - buf->data);
    print_debug_warn("moving %zu bytes\n", data_len);
    memmove(buf->data, header_end, data_len + 1);

    return (buf->at = data_len);
}
