#ifndef OPERATION_H
#define OPERATION_H
#include <string>
#include <list>

using namespace std;

class Operation
{
public:
	char chr;
	int pos;
	bool isInsert;
	int id;
	int serverId;
	int prevId;

	Operation(string message);
	Operation(bool isInsert, int pos, int prevId, char chr = 255);

	void applyOperation(string &text);
	string toStr();
};

extern bool updateOperation(list<Operation> &history, Operation &operation, bool secondCall = false);
#endif 