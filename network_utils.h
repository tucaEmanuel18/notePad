#pragma once

#include <string.h>
#include <iostream>
#include <unistd.h>

using namespace std;

extern int Send(int fd, string command);
extern string Receive(int fd, int message_max_size = 1024);
