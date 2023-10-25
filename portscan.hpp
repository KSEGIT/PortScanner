#ifndef PORTSCAN_HPP
#define PORTSCAN_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <vector>
#include <algorithm>

using namespace std;

// Global IP address variable from main.cpp file
extern const char * ipAddress;
// Verbose switch
extern bool verbose;

//extern void getBanner(const char * ipAddress);
//extern void startBanner();

// Data structure for port arguments
struct port_args {
    int start;
    int end;
    int sockfd;
    struct sockaddr_in* tower;
};

// List of open ports
static vector<int> openPorts;

// Protects shared data from being simultaneously accessed by multiple threads.
static mutex vecMutex;

#endif