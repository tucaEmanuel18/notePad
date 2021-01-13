#include <string>
#include <vector>
#include <list>
#include <map>
#include <mutex>
#include <chrono>
#include <sstream>

#include "operation.h"

using namespace std;

struct Client
{
	int fd;
	list<Operation> history;
};

class Document
{
public:
	string content;
	string name;
	vector<Client> clients;
	mutex lock;
	int shared;
	int prevId;
	int prevIdCount;
	

	Document(string name);
	Document(const Document &doc);
	void ApplyOperation(int fd, string command);
	void RemoveClient(int fd);
	int AddClient(int fd);
};


// Acest .h si .cpp-ul asociat a fost inspirat de fisierul document.h si .cpp-ul asociat 
// din cadrul proiectului urmator: https://github.com/buterchiandreea/CollaborativeNotepad.git