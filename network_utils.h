#pragma once

#include <string.h>
#include <iostream>
#include <unistd.h>

using namespace std;

extern int Write(int fd, string command);
extern string Read(int fd, int message_max_size = 1024);