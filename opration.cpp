#include "operation.h"
#include <string>
#include <list>
#include <iterator>
#include <sstream>

using namespace std;

Operation::Operation(string message)
{
	;
}

Operation::Operation(bool isInsert, int prevId, int pos, char chr)
{
	;
}

void Operation::applyOperation(string &text)
{
	;
}

string Operation::toStr()
{
	;
}

bool updateOperation(list<Operation> &history, Operation &operation, bool secondCall)
{
	;
}