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

// Startting cURL to get banner and certyficates
void getBanner(const char * ipAddress);

// Verbose flag from main
extern bool verbose;

#endif