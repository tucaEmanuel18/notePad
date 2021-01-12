#include "network_utils.h"

int Send(int fd, string command)
{
	int size = command.size();
	if(write(fd, &size, sizeof(int)) == -1)
	{
		return errno;
	}
	if(write(fd, command.c_str(), command.size()) == -1)
	{
		return errno;
	}
	return 0;
}

string Receive(int fd, int messageMaxSize)
{
    int length;
    int readNrOfBytes;
    if((readNrOfBytes = read(fd, &length, sizeof(int))) <= 0)
	{
		throw(std::ios_base::failure(strerror(errno)));
	}
    char* buffer = new char[messageMaxSize];
    string message = "";
	int nrOfReadBytes = 0;
    while(nrOfReadBytes < length)
	{	
        int readNrOfBytes = read(fd, buffer, min(messageMaxSize - 1, length - nrOfReadBytes));
        if(readNrOfBytes <= 0)
		{
			delete[] buffer;
			throw std::ios_base::failure(strerror(errno));
		}
        buffer[readNrOfBytes] = '\0';
        message += buffer;
        nrOfReadBytes += readNrOfBytes;
	}
	delete[] buffer;
    return message;
}

