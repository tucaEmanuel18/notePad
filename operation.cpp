#include "operation.h"
#include <string>
#include <list>
#include <iterator>
#include <sstream>

using namespace std;

Operation::Operation(string command)
{
	string opName;
	stringstream ss(command);
	int chrId;
	ss >> opName >> this->id >> this->prevId >> this->serverId >> this->pos >> chrId;
	this->chr = (char) chrId;
	this->isInsert = opName == "insert";
}

Operation::Operation(bool isInsert, int prevId, int pos, char chr)
{
	this->prevId = prevId;
    this->id = 0;
    this->serverId = 0;
    this->isInsert = isInsert;
    this->pos = pos;
    this->chr = chr;
}

void Operation::applyOperation(string &text)
{
	if(isInsert)
	{
		string toAdd = "";
		toAdd += this->chr;
		text.insert(this->pos, toAdd);
	}
	else
	{
		text.erase(this->pos, 1);
	}
}

string Operation::toStr()
{
	stringstream ss;
	if(this->isInsert)
	{
		ss << "insert";
	}
	else
	{
		ss << "delete";
	}
	ss << " " << this->id << " " << this->prevId << " " << this->serverId << " " << this->pos;
	ss << " " << (int)this->chr;
	return ss.str();
}

bool updateOperation(list<Operation> &history, Operation &operation, bool secondCall)
{
	bool deleteCase = false;

    if (!secondCall) {
        while(!history.empty() && history.front().id <= operation.prevId) {
            history.pop_front(); //indiferent de operatiile care vin de la clientul vecin, daca au id mai mic, nu influenteaza operatia asta si cele ce urmeaza
        }
    }

    for(list<Operation>::iterator it=history.begin(); it != history.end(); ++it) {
        Operation op = *it;

        if(op.isInsert) {
            if(operation.isInsert) {
                if(op.pos == operation.pos) {
                    if(op.id > operation.id) {
                        operation.pos++;
                    }
                    else {
                        if(op.id == operation.id) {
                            if(op.serverId > operation.serverId) {
                                operation.pos++;
                            }
                        }
                    }
                } else {
                    if(op.pos < operation.pos) {
                        operation.pos++;
                    }
                }
            } else {
                if(op.pos <= operation.pos) {
                    operation.pos++;
                }
            }
        } else {
            if(!operation.isInsert) {
                if(op.pos == operation.pos) {
                    deleteCase = true;
                    break;
                } else {
                    if(op.pos < operation.pos) {
                        operation.pos--;
                    }
                }
            } else {
                if(operation.isInsert) {
                    if(op.pos < operation.pos) {
                        operation.pos--;
                    }
                }
            }
        }

        operation.prevId = op.id;
    }

    return !deleteCase;
}