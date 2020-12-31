#include "document.h"
#include "network_utils.h"

Document::Document(const Document &doc)
{
	this->documentName = doc.documentName;
	this->lastId = doc.lastId;
	this->lastIdCount = doc.lastIdCount;
	this->documentText = doc.documentText;
	this->clients = doc.clients;
	this->lock.unlock();
}

Document::Document(string name)
{
	this->documentName = name;
	this->lastId = -1;
	this->lastIdCount = 0;
	this->lock.unlock();
}

int Document::AddClient(int fd)
{
	Client new_client;
	new_client.fd = fd;
	int prevId = 0;
	if(!this->clients.empty())
	{
		prevId = this->clients[0].history.back().id;
	}
	this->clients.push_back(new_client);

	return prevId;
}

void Document::DisconnectClient(int fd)
{
	auto found = this->clients.end();
	for(auto it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if(it->fd == fd)
		{
			found == it;
			break;
		}

		if(found != this->clients.end())
		{
			this->clients.erase(found);
		}

	}
}

void Document::ApplyOperation(int fd, string command)
{
	Operation op(command);
	printf("Apply Operation: %s, %c\n", command.c_str(), (int) op.chr);

	bool deleteCase = false;

	this->lock.lock();
	op.id = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

	if(op.id > lastId)
	{
		lastId = op.id;
		lastIdCount = 0;
	}
	op.serverId = lastIdCount;
	lastIdCount++;

	bool hasPeer = false;

	for(auto client : this->clients)
	{
		if(client.fd == fd) // in cazul clientului care a efectuat operatia
		{
			client.history.push_back(op);
		}
		else
		{
			Write(client.fd, op.toStr()); // trimitem celuilalt client operatia
			printf("Send op to other client: %d %s\n", client.fd, op.toStr().c_str());
			hasPeer = true;

			if(!updateOperation(client.history, op))
			{
				deleteCase = true; // dublu delete
			}
		}

	}
	if(!deleteCase)
	{
		op.applyOperation(this->documentText);
	}
	this->lock.unlock(); // serverul nu accepta alta operatie in timpul updatului
	stringstream ss;
	ss << op.id << " "<<op.serverId << " " << hasPeer;
	Write(fd, ss.str());
}