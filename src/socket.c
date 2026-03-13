#include "socket.h"

#include <arpa/inet.h> // inet_addr()
#include <chlsdl-modules/chlsdl-common/print.h>
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
