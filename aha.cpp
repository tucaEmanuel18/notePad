#include<iostream>
#include <string>
using namespace std;

int main()
{
	string command = "sign emanuel tuca";
    size_t userNamePosition = command.find(" ");
    string userName = command.substr(userNamePosition + 1);
    size_t passwordPosition = userName.find(" ");
    string password = userName.substr(passwordPosition + 1);
    userName.erase(passwordPosition);
    printf("userName =%s| password =%s ", userName.c_str(), password.c_str());
	return 0;
}