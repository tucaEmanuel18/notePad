#ifndef HACK_CONNECT_H
#define HACK_CONNECT_H

#include <sys/socket.h>

int sock_connect(int fd, const struct sockaddr *server, socklen_t socklen) {
    return connect(fd, server, socklen);
}

#endif // HACK_CONNECT_H
