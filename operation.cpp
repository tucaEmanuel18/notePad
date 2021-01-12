#include "operation.h"
#include <string>
#include <list>
#include <iterator>
#include <sstream>

using namespace std;

Operation::Operation(string command)
{
	string operationName;
    int characterId;

	stringstream msg(command);
	msg >> operationName >> this->id >> this->prevId >> this->serverId >> this->position >> characterId;
	this->character = (char) characterId;
	this->isInsert = operationName == "insert";
}

Operation::Operation(bool isInsert, int prevId, int position, char character)
{
    this->isInsert = isInsert;
	this->prevId = prevId;
    this->position = position;
    this->serverId = 0;
    this->id = 0;
    this->character = character;
}

void Operation::applyOperation(string &text)
{
	if(isInsert)
	{
		string toInsert = "";
		toInsert += this->character;
		text.insert(this->position, toInsert);
	}
	else
	{
		text.erase(this->position, 1);
	}
}

string Operation::toStr()
{
	stringstream msg;
	if(this->isInsert)
	{
        msg << "insert";
	}
	else
	{
        msg << "delete";
	}
	msg << " " << this->id << " " << this->prevId << " " << this->serverId << " " << this->position;
	msg << " " << (int)this->character;
	return msg.str();
}

bool transformOperation(list<Operation> &pastOpList, Operation &thisOp, bool secondCall)
{
	bool delCase = false;
    if (!secondCall) 
    {
        while(!pastOpList.empty() && pastOpList.front().id <= thisOp.prevId) 
        {
            //operatiile care vin de la celalalt client, daca au id mai mic, nu influenteaza nici operatia aceasta si nici pe cele ce urmeaza
            pastOpList.pop_front(); 
        }
    }

    for(auto it = pastOpList.begin(); it != pastOpList.end(); ++it) 
    {
        Operation otherOp = *it;

        if(otherOp.isInsert) 
        {
            if(thisOp.isInsert) 
            {   
                //daca ambele sunt insert verificam pozitiile
                if(otherOp.position == thisOp.position) 
                {
                    if(otherOp.id > thisOp.id) 
                    {
                        thisOp.position++;
                    }
                    else 
                    {
                        if(otherOp.id == thisOp.id) 
                        {
                            if(otherOp.serverId > thisOp.serverId) 
                            {
                                thisOp.position++;
                            }
                        }
                    }
                } 
                else 
                {
                    if(otherOp.position < thisOp.position) 
                    {
                        thisOp.position++;
                    }
                }
            } 
            else 
            {
                if(otherOp.position <= thisOp.position)
                {
                    thisOp.position++;
                }
            }
        } 
        else 
        {
            if(!thisOp.isInsert) 
            {
                if(otherOp.position == thisOp.position)
                {
                    delCase = true;
                    break;
                } 
                else 
                {
                    if(otherOp.position < thisOp.position) 
                    {
                        thisOp.position--;
                    }
                }
            }
            else 
            {
                if(thisOp.isInsert)
                {
                    if(otherOp.position < thisOp.position) 
                    {
                        thisOp.position--;
                    }
                }
            }
        }
        thisOp.prevId = otherOp.id;
    }
    return !delCase;
}
