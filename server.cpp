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
#include "operation.h"
#include "network_utils.h"

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

int serverLoop(string serverAdr, int port, int backlog);
void clientLoop(int clientFd, map<string, string>* clientsDict, map<int, string>* connectedClients, map<string, vector<string>>* clientAcces, map<string, Document>* documentsDict, map<int, string>* openedDocuments);

int main()
{
	printf("Binding to %s:%d\n", SERVER_ADDRESS, PORT);
	serverLoop(SERVER_ADDRESS, PORT, BACKLOG);
}

int serverLoop(string serverAdr, int port, int backlog)
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
  		perror ("[server]Eroare la bind().\n");
        return errno;
  	} 

  	// punem serverul sa asculte 
  	if(listen(sd, BACKLOG) == -1)
  	{
  		perror ("[SERVER] Error at listen! \n");
        return errno;
  	}

    //declararea  structurilor de date ce vor fi utilizate de toti clientii
   map<string, string> *clientsDict = new map<string, string>();
   map<string, vector<string>>* clientAcces = new map<string, vector<string>>;
   map<int, string> *connectedClients = new map<int, string>();
   map<string, Document> *documentsDict = new map<string, Document>();
   map<int, string> *openedDocuments = new map<int, string>();

  	// servim in mod concurent clientii
  	while(1)
  	{
  		int clientFd;
  		int length = sizeof (from);
  		fflush (stdout);

  		if( (clientFd = accept(sd, (struct sockaddr*) &from, (socklen_t *) &length)) < 0)
  		{
  			perror ("[SERVER] Error at accept! \n");
            continue;
  		}
  		// s-a realizat conexiunea
  		printf("[SERVER] Client with fd %d was connected\n", clientFd);

  		new thread(clientLoop, clientFd, clientsDict, connectedClients, clientAcces, documentsDict, openedDocuments);
  	}

}

//map<string, string>* clientsDict, map<int, string>* connectedClients,
//clientsDict, connectedClients,
void clientLoop(int clientFd, map<string, string>* clientsDict, map<int, string>* connectedClients, map<string, vector<string>>* clientAcces, map<string, Document>* documentsDict, map<int, string>* openedDocuments)
{

  printf("[SERVER THREAD] Thread with fd %d started \n", clientFd);
  while(1)
  {
    string command;
    try
    {
        command = Read(clientFd, 1024);
    }
    catch (std::ios_base::failure exception)
    {
      printf("[SERVER THREAD] %s \n", exception.what());
      break;
    }
    printf("[SERVER THREAD] Command received from client: %s\n", command.c_str());

    if(command.compare(0, 4 ,"sign") == 0)
    {
      size_t userNamePosition = command.find(" ");
      string userName = command.substr(userNamePosition + 1);
      size_t passwordPosition = userName.find(" ");
      string password = userName.substr(passwordPosition + 1);
      userName.erase(passwordPosition);

      if(clientsDict->find(userName) != clientsDict->end())
      {
        Write(clientFd, "ERROR: This username already exists!");
        continue;
      }

      clientsDict->insert(make_pair(userName, password));
      vector<string> accesDocument;
      clientAcces->insert(make_pair(userName, accesDocument));
      Write(clientFd, "Ok!");
    }
    else if(command.compare(0, 3, "log") == 0)
    {
      printf("Am primit comanda: %s\n", command.c_str());
      size_t userNamePosition = command.find(" ");
      string userName = command.substr(userNamePosition + 1);
      size_t passwordPosition = userName.find(" ");
      string password = userName.substr(passwordPosition + 1);
      userName.erase(passwordPosition);
      printf("Am obtinut userName: %s, password: %s\n", userName.c_str(), password.c_str());
      auto findUser = clientsDict->find(userName);
      if(findUser == clientsDict->end())
      {
        Write(clientFd, "ERROR1: This username doesn't exists!");
        continue;
      }

      if(password.compare(findUser->second) == 0)
      {
        connectedClients->insert(make_pair(clientFd, userName));
        Write(clientFd, "Ok!");
      }
      else
      {
        Write(clientFd, "ERROR2: The password is wrong!");
        continue;
      }
    }
    else if(command == "list")
          {
            // aflam userName-ul clientului
            auto findUser = connectedClients->find(clientFd);
            string userName = findUser->second;
            printf("Am aflat userName: %s pentru client %d\n", userName.c_str(), clientFd);


            // obtinem vectorul cu numele documentelor la care clientul cu userName-ul gasit are acces.
            auto findDoc = clientAcces->find(userName);
            vector<string> accesDocument = findDoc->second;

            stringstream ss;
            ss << accesDocument.size();
            printf("%lu, %s\n", accesDocument.size(), ss.str().c_str());
            Write(clientFd, ss.str()); // transmitem catre client numarul de documente existente

            // transmitem documentele
            for(auto it = accesDocument.begin(); it != accesDocument.end(); it++)
            {
              Write(clientFd, *it);
            }

          }
    else if(command.compare (0, 6, "create") == 0)
          {
            size_t namePosition = command.find(" ");
            string documentName = command.substr(namePosition + 1);
            printf("Document: %s\n", documentName.c_str());

            if(documentsDict->find(documentName) != documentsDict->end())
            {
              Write(clientFd, "ERROR: File with the same name already exists on server! Choose another filename!");
              continue;
            }
            documentsDict->insert(make_pair(documentName, Document(documentName)));
            auto findName = connectedClients->find(clientFd);
            string userName = findName->second;
            auto findAcces = clientAcces->find(userName);
            findAcces->second.push_back(documentName);
            Write(clientFd, "Ok!");
          }
    else if(command.compare(0, 6, "remove") == 0)
          {
            //aflam numele fisierului ce trebuie sters
            size_t namePosition = command.find(" ");
            string documentName = command.substr(namePosition + 1);
            printf("Document: %s\n", documentName.c_str());

            //aflam numele clientului
            auto findName = connectedClients->find(clientFd);
            string userName = findName->second;

            //restrictionam accesul clientului la acest fisier
            auto findAcces = clientAcces->find(userName); 
            for(auto findDocumentAcc = findAcces->second.begin(); findDocumentAcc != findAcces->second.end(); findDocumentAcc++)
            {
              if(documentName.compare(*findDocumentAcc) == 0)
              {
                findAcces->second.erase(findDocumentAcc);
                findDocumentAcc--;
              }
            }

            // scadem numarul de partajari ale documentului
            auto findDocument = documentsDict->find(documentName);
            if(--findDocument->second.shared == 0)
            {
              documentsDict->erase(findDocument);
            }
          }
    else if(command.compare(0, 5,"share") == 0)
          {
            size_t documentNamePosition = command.find(" ");
            string documentName = command.substr(documentNamePosition + 1);
            size_t friendUserNamePosition = documentName.find(" ");
            string userName = documentName.substr(friendUserNamePosition + 1);
            documentName.erase(friendUserNamePosition);



            auto findUserAcces = clientAcces->find(userName);
            if(findUserAcces != clientAcces->end())
            {
              findUserAcces->second.push_back(documentName);

              auto findDocument = documentsDict->find(documentName);
              findDocument->second.shared++;
              Write(clientFd, "OK!");
            }
            else
            {
              Write(clientFd, "ERROR: there is no user with the specified username.");
            }
          }
    else if(command.compare(0, 4, "open") == 0)
          {
            string space = " ";
            size_t namePosition = command.find(space);
            string documentName = command.substr(namePosition + 1);
            printf("Document: %s\n", documentName.c_str());

            auto doc = documentsDict->find(documentName);
            if(doc != documentsDict->end())
            {
              if(doc->second.clients.size() < 2)
              {
                int lastOpId = doc->second.AddClient(clientFd);
                openedDocuments->insert(make_pair(clientFd, documentName));

                stringstream ss;
                ss << lastOpId << " " << doc->second.documentText;
                Write(clientFd, ss.str());
              }
              else
              {
                Write(clientFd, "ERROR: There are already two clients!");
              }
            }
          }
      else if(command.compare(0, 6, "insert") == 0 || command.compare(0, 6, "delete") == 0)
            {
              auto opened_doc = openedDocuments->find(clientFd);

              if(opened_doc != openedDocuments->end())
                {
                  string docName = opened_doc->second;
                  printf("docName: %s\n", docName.c_str());
                  auto doc = documentsDict->find(docName);
                  if(doc != documentsDict->end())
                  {
                    doc->second.ApplyOperation(clientFd, command);
                  }
                }
            }
  }
  printf("Client disconected: %d\n", clientFd);
  auto opened_doc = openedDocuments->find(clientFd);
  if(opened_doc != openedDocuments->end())
  {
    string docName = opened_doc->second;
    auto doc = documentsDict->find(docName);
    doc->second.DisconnectClient(clientFd);
    openedDocuments->erase(opened_doc);
  }
}