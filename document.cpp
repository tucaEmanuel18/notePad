#include "document.h"
#include "network_utils.h"

Document::Document(const Document &doc)
{
	this->documentName = doc.documentName;
	this->lastId = doc.lastID;
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
	;
}

void Document::DisconnectClient(int fd)
{
	;
}

void Document::ApplyOperation(int fd, string command)
{
	;
}