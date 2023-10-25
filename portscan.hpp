#ifndef PORTSCAN_HPP
#define PORTSCAN_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unordered_map>
#include <thread>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <vector>
#include <algorithm>

using namespace std;

//global ip address variable from main.cpp file
extern const char * ipAddress;

//Datastructure for port arguments
struct port_args {
    int start;
    int end;
    int sockfd;
    struct sockaddr_in* tower;
};

//Verbose switch
static bool verbose = false;

static unordered_map<int, string> portMap;
//List of open ports
static vector<int> openPorts;
//Protects shared data from being simultaneously accessed by multiple threads.
static mutex vecMutex;

#endif