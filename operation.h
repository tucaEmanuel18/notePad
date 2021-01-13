#ifndef OPERATION_H
#define OPERATION_H
#include <string>
#include <list>

using namespace std;

class Operation
{
public:
	char character;
	int position;
	bool isInsert;
	int id;
	int serverId;
	int prevId;

	Operation(bool isInsert, int position, int prevId, char character = 255);
	Operation(string command);


	void applyOperation(string &text);
	string toStr();
};

extern bool transformOperation(list<Operation> &pastOpList, Operation &thisOp, bool secondCall = false);
#endif 

// Acest .h si .cpp-ul asociat a fost inspirat de fisierul operation.h si .cpp-ul asociat 
// din cadrul proiectului urmator: https://github.com/buterchiandreea/CollaborativeNotepad.git