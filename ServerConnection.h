#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <stdlib.h>

using std::string;


class ServerConnection
{
public:
    ServerConnection(int port);
    int Connect();
    int WriteCommand(string command);
    string ReadCommand(int MESSAGE_MAX_CHUNK_SIZE = 1024);
    int CloseServerConnection();

private:
    const static char *ADDR;
    int fd;
    int port;

};

#endif // CLIENT_H
