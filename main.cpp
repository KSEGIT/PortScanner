
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include "banner.cpp" //For simple makefile build, TODO: prepare proper structure
#include "portscan.cpp" //For simple makefile build, TODO: prepare proper structure

using namespace std;

// Global IP address variable
const char * ipAddress;

// Verbose switch
bool verbose = false;

// Print help message to the console
void help(){
    printf(
    "Please specify a flag:\n"
    "  '-v' - Verbose mode\n"
    "  '-q' - Quick scan for tls/ssl related ports\n"
    "  '-s' - Scan system ports\n"
    "  '-u' - Scan user ports\n"
    "  '-p' - Scan private ports\n"
    "  '-a' - Scan all ports\n"
    "  '-h' - Display this help message\n"
    );
}
/* 
void startBanner() {
    getBanner(ipAddress);
} */

int main(int argc, char* argv[]){

    // Print help when no arguments is provided
    if (argc == 1){
        help();
        return 1;
    }

    // Print info when a user uses only "-" symbol as an argument
    if (strcmp(argv[1], "-") == 0){
        printf("Please provide correct arguments!\n");
        help();
        return 1;
    }

    // Prompting for IP address input
    if (strcmp(argv[2], "") == 0){
        printf("Please provide Ip address.\n");
        help();
        return 1;
    }

    // Printing help when too many arguments provided
    if (argc > 3){
        printf("Too many arguments provided, use help for more information.\n");
        help();
        return 1;
    } 

    // Setting main variables
    int opt, startPort, endPort;
    // Definition of global IP address variable
    ipAddress = argv[2];
    
    // Start main loop
    while((opt = getopt(argc, argv, "vqsupah")) != -1){
        switch (opt){
            case 'v':
                verbose = true;
                break;
            case 'q':
                startPort = 440;
                endPort = 450;
                thread_handler(ipAddress, startPort, endPort, (char) opt);
                //getBanner(ipAddress);
                break;    
            case 's':
                startPort = 0;
                endPort = 1023;
                thread_handler(ipAddress, startPort, endPort, (char) opt);
                break;
            case 'u':
                startPort = 1024;
                endPort = 49151;
                thread_handler(ipAddress, startPort, endPort, (char) opt);
                break;
            case 'p':
                startPort = 49152;
                endPort = 65535;
                thread_handler(ipAddress, startPort, endPort, (char) opt);
                break;
            case 'a':
                startPort = 0;
                endPort = 65535;
                thread_handler(ipAddress, startPort, endPort, (char) opt);
                break;
            case 'h':
                help();
                break;

            default:
                help();
                return 1;
        }
    }
    return 0;
}