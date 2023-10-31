// #PortScanner
// Daniel Kiska
// 26.10.2023

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
#include <netdb.h>

using namespace std;

// Global IP address variable from main.cpp file
extern const char * g_ipAddress;

// Verbose switch
extern bool g_verbose;

// Get banner funtion from banner.hpp
extern void getBanner(const char * ipAddress);

// List of open ports
static vector<int> openPorts;

// Protects shared data from being simultaneously accessed by multiple threads.
static std::mutex vecMutex;

// Thread handler function from portscan.hpp
void thread_handler(int start, int end, char flag);

#endif
