#include "document.h"
#include "network_utils.h"

Document::Document(string name)
{
	this->name = name;
	this->shared = 1;
	this->prevId = -1;
	this->prevIdCount = 0;
	this->lock.unlock();
}

Document::Document(const Document &doc)
{
	this->name = doc.name;
	this->prevId = doc.prevId;
	this->shared = doc.shared;
	this->prevIdCount = doc.prevIdCount;
	this->content = doc.content;
	this->clients = doc.clients;
	this->lock.unlock();
}

void Document::ApplyOperation(int fd, string command)
{
	Operation operation(command);
	printf("Apply Operation: %s, %c\n", command.c_str(), (int) operation.character);

	bool delCase = false;

	this->lock.lock();
	operation.id = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

	if(operation.id > prevId)
	{
		prevId = operation.id;
		prevIdCount = 0;
	}
	operation.serverId = prevIdCount;
	prevIdCount++;

	bool hasPeer = false;
	for(auto client : this->clients)
	{
		if(client.fd == fd) // in cazul clientului care a efectuat operatia
		{
			client.history.push_back(operation);
		}
		else
		{
			Send(client.fd, operation.toStr()); // trimitem celuilalt client operatia
			printf("Send op to other client: %d %s\n", client.fd, operation.toStr().c_str());
			hasPeer = true;

			if(!transformOperation(client.history, operation))
			{
				delCase = true; // dublu delete
			}
		}

	}
	if(!delCase)
	{
		operation.applyOperation(this->content);
	}
	this->lock.unlock(); // serverul nu accepta alta operatie in timpul update - ului
	stringstream ss;
	ss << operation.id << " "<<operation.serverId << " " << hasPeer;
	Send(fd, ss.str());
}

void Document::RemoveClient(int fd)
{
	auto foundClient = this->clients.end();
	for(auto iterator = this->clients.begin(); iterator != this->clients.end(); iterator++)
	{
		if(iterator->fd == fd)
		{
			foundClient = iterator;
			break;
		}
	}
	if(foundClient != this->clients.end())
	{
		this->clients.erase(foundClient);
	}
}

int Document::AddClient(int fd)
{
	Client newClient;
	newClient.fd = fd;
	int prevId = 0;
	if(!this->clients.empty())
	{
		prevId = this->clients[0].history.back().id;
	}
	this->clients.push_back(newClient);

	return prevId;
}


