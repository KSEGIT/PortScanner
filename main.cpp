#include "pscan.cpp"
#include <iostream>

//Print help to the console
void help(){
    printf(
    "Please specify a flag:\n"
    "  '-v' - Verbose mode\n"
    "  '-q' - Quick scan for tls/ssl\n"
    "  '-s' - Scan system ports\n"
    "  '-u' - Scan user ports\n"
    "  '-p' - Scan private ports\n"
    "  '-a' - Scan all ports\n"
    "  '-h' - Display this help message\n"
    );
}

int main(int argc, char* argv[]){

    //Print help when no arguments provided
    if (argc == 1){
        help();
        return 1;
    }

    //Print info when user use only - as a argument
    if (strcmp(argv[1], "-") == 0){
        printf("Please provide correct arguments!\n");
        help();
        return 1;
    }

    if (strcmp(argv[2], "") == 0){
        printf("Please provide Ip address.\n");
        help();
        return 1;
    }

    if (argc > 3){
        printf("Too many arguments use help for more information.\n");
        help();
        return 1;
    } 

    //startPort main loop
    int opt, startPort, endPort;
    string ipAddress = argv[2];
    while((opt = getopt(argc, argv, "vqsupah")) != -1){
        switch (opt){
            case 'v':
                verbose = true;
                break;
            case 'q':
                startPort = 440;
                endPort = 450;
                thread_handler(startPort, endPort, (char) opt);
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