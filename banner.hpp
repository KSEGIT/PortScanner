#ifndef BANNER_HPP
#define BANNER_HPP

#include <iostream>
#include <string>
#include <cerrno>       
#include <cstring>      
#include <cstdlib>
#include <curl/curl.h>

// TODO more detailed ssl information
// use -lssl -lcrypto
//#include <openssl/ssl.h>

using namespace std;

// Starting cURL to get banner and certificates
void getBanner(const char * ipAddress);

// Verbose flag from main
extern bool g_verbose;

#endif
