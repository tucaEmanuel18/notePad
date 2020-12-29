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
	string documentText;
	string documentName;
	vector<Client> clients;
	int lastId;
	int lastIdCount;
	mutex lock;

	Document(const Document &doc);
	Document(string name);

	int AddClient(int fd);
	void DisconnectClient(int fd);

	void ApplyOperation(int fd, string command);
};