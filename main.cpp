
// #PortScanner
// Daniel Kiska
// 26.10.2023

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include "portscan.hpp"
#include "banner.hpp"

using namespace std;

// Global IP address variable
const char * g_ipAddress;

// Global verbose switch
bool g_verbose = false;

// Print help message to the console
void help(){
    printf(
    "Please specify a flag:\n"
    "  '-v' - Verbose mode\n"
    "  '-q' - Quick scan for TLS/SSL related ports\n"
    "  '-s' - Scan system ports\n"
    "  '-u' - Scan user ports\n"
    "  '-p' - Scan private ports\n"
    "  '-a' - Scan all ports\n"
    "  '-h' - Display this help message\n"
    "Remember to provide IP/hostname as argument\n"
    "Example use: ./portscan -q 192.168.1.1\n"
    );
}

int main(int argc, char* argv[]){

    // Print help when no arguments are provided
    if (argc == 1){
        help();
        return 1;
    }

    // Print help when no arguments are provided
    if (argc == 2){
        help();
        return 1;
    }

    // Printing help when too many arguments provided
    if (argc > 3){
        printf("Too many arguments provided, use help for more information.\n");
        help();
        return 1;
    } 

    // Print info when a user uses only "-" symbol as an argument
    if (strcmp(argv[1], "-") == 0){
        printf("Please provide correct flags!\n");
        help();
        return 1;
    }  

    // Prompting for IP address input
    if (strcmp(argv[2], "") == 0){
        printf("Please provide Ip address.\n");
        help();
        return 1;
    }

    // Setting main variables
    int opt;
    unsigned int startPort, endPort;
    
    // Definition of global IP address variable
    g_ipAddress = argv[2];
    
    // Start main loop
    while((opt = getopt(argc, argv, "vqsupah")) != -1){
        switch (opt){
            case 'v':
                g_verbose = true;
                break;
            case 'q':
                scanSingleOpenPort (80, (char) opt);
                scanSingleOpenPort (443, (char) opt);
                scanSingleOpenPort (445, (char) opt);
                runPrintPort ((char) opt);
                break;    
            case 's':
                startPort = 0;
                endPort = 1023;
                thread_handler(startPort, endPort, (char) opt);
                break;
            case 'u':
                startPort = 1024;
                endPort = 49151;
                thread_handler(startPort, endPort, (char) opt);
                break;
            case 'p':
                startPort = 49152;
                endPort = 65535;
                thread_handler(startPort, endPort, (char) opt);
                break;
            case 'a':
                startPort = 0;
                endPort = 65535;
                thread_handler(startPort, endPort, (char) opt);
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
