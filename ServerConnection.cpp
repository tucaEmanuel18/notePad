#include "ServerConnection.h"
#include "network_utils.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

using std::string;

const char* ServerConnection::ADDR = "0.0.0.0";


ServerConnection::ServerConnection(int port) {
    this->port = port;
}

int ServerConnection::Connect()
{

    struct sockaddr_in server;

    /* create socket */
    if ((this->fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
      return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ServerConnection::ADDR);
    server.sin_port = htons (port);

    return connect (this->fd, (const struct sockaddr *) &server, (socklen_t)sizeof (server)) == -1 ? errno : 0;
}

int ServerConnection::SendCommand(string command) {

    return Send(this->fd, command);
}

string ServerConnection::ReceiveCommand(int MESSAGE_MAX_CHUNK_SIZE) {

    return Receive(this->fd, MESSAGE_MAX_CHUNK_SIZE);

}

int ServerConnection::CloseServerConnection() {

   return close(this->fd) == -1 ? errno : 0;
}
