#include "network_utils.h"



int Write(int fd, string command)
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

string Read(int fd, int message_max_size)
{
	int size;
	int readBytes;
	if((readBytes = read(fd, &size, sizeof(int))) <= 0)
	{
		throw(std::ios_base::failure(strerror(errno)));
	}
	char* buffer = new char[message_max_size];
	string command = "";
	int nrOfReadBytes = 0;
	while(nrOfReadBytes < size)
	{	
		int readBytes = read(fd, buffer, min(message_max_size - 1, size - nrOfReadBytes));
		if(readBytes <= 0)
		{
			delete[] buffer;
			throw std::ios_base::failure(strerror(errno));
		}
		buffer[readBytes] = '\0';
		command += buffer;
		nrOfReadBytes += readBytes;
	}
	delete[] buffer;
	return command;
}

