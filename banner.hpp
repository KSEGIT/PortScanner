#ifndef BANNER_HPP
#define BANNER_HPP

#include <iostream>
#include <curl/curl.h>

// Initialize libcurl
CURL* curl = curl_easy_init();

//get the host response
static void getBanner(const char * ipAddress);

#endif