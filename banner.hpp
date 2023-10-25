#ifndef BANNER_HPP
#define BANNER_HPP

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <cerrno>       
#include <cstring>      
#include <cstdlib>

using namespace std;

// Initialize libcurl
CURL* curl = curl_easy_init();

//get the host response
void getBanner(const char * ipAddress);

//gettint verbose flag from main
extern bool verbose;

#endif