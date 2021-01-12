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

	Operation(string command);
    Operation(bool isInsert, int position, int prevId, char character = 255);

	void applyOperation(string &text);
	string toStr();
};

extern bool transformOperation(list<Operation> &pastOpList, Operation &thisOp, bool secondCall = false);
#endif 
