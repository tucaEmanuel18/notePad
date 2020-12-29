#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <map>
#include <vector>
#include <thread>
#include <sstream>
#include <mutex>
#include "document.h"

using namespace std;

#ifndef PORT
#define PORT 2000
#endif

#ifndef SERVER_ADDRESS
#define SERVER_ADDRESS "127.0.0.1"
#endif

#ifndef BACKLOG
#define BACKLOG 5
#endif

extern int errno;

void serverLoop(string serverAdr, int port, int backlog);
void clientLoop(int clientFd, map<string, Document> *documentsDict, map<int, string>* openedDocuments);

int main()
{
	printf("Binding to %s:%d\n", SERVER_ADDRES, PORT);
	serverLoop(SERVER_ADDRES, PORT, BACKLOG);
}

void serverLoop(string serverAdr, int port, int backlog)
{
	struct sockaddr_in server; // structura folosita de server
	struct sockaddr_in from; // pentru a retine informatiile clientilor ce se conecteaza
	int sd; //descriptorul de socket

	/* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }

    // utilizarea optiunii SO_REUSEADDR
    int on = 1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    //pregatirea structurilor de date
    bzero (&server, sizeof (server));
  	bzero (&from, sizeof (from));

  	// server
  	server.sin_family = AF_INET;
  	server.sin_addr.s_addr = htonl (INADDR_ANY);
  	server.sin_port = htons(PORT);

  	// atasam socketului adresa obtinuta
	if(bind(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1)
  	{
  		pperror ("[server]Eroare la bind().\n");
        return errno;
  	} 

  	// punem serverul sa asculte 
  	if(listen(st, BACKLOG) == -1)
  	{
  		perror ("[SERVER] Error at listen! \n");
        return errno;
  	}

  	//declararea  structurilor de date ce vor fi utilizate de toti clientii
  	map<string, Document> * documentsDict = new map<string, Document>();
  	map<int, string> *openedDocuments = new map<int, string>();
  	// servim in mod concurent clientii
  	while(1)
  	{
  		int clientFd;
  		int length = sizeof (from);
  		fflush (stdout);

  		if( (cliendFd = accept(sd, (struct sockadd*) &from, (socklen_t *) &length)) < 0)
  		{
  			perror ("[SERVER] Error at accept! \n");
            continue;
  		}
  		// s-a realizat conexiunea
  		printf("[SERVER] Client with fd %d was connected\n", cliendFd);

  		new thread(clientLoop, cliendFd, documentsDict, openedDocuments);

  	}

}