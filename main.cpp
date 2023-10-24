#include "pscan.cpp"
#include <iostream>

//Print help to the console
void help(){
    printf(
    "Please specify a flag:\n"
    "  '-s' - Scan system ports\n"
    "  '-u' - Scan user ports\n"
    "  '-p' - Scan private ports\n"
    "  '-a' - Scan all ports\n"
    "  '-h' - Display this help message\n"
    "  '-t' - Display this help message\n"
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

    //startPort main loop
    int c, startPort, endPort;
    string ipAddres;
    while((c = getopt(argc, argv, "vsupaht")) != -1){
        switch (c){
            case 'v':
                verbose = true;
            case 's':
                startPort = 0;
                endPort = 1023;
                thread_handler(startPort, endPort, (char) c);
                break;
            case 'u':
                startPort = 1024;
                endPort = 49151;
                thread_handler(startPort, endPort, (char) c);
                break;
            case 'p':
                startPort = 49152;
                endPort = 65535;
                thread_handler(startPort, endPort, (char) c);
                break;
            case 'a':
                startPort = 0;
                endPort = 65535;
                thread_handler(startPort, endPort, (char) c);
                break;
            case 'h':
                help();
                break;
            case 't':
                startPort = 440;
                endPort = 450;
                thread_handler(startPort, endPort, (char) c);
                break;
            default:
                help();
                return 1;
        }
    }
    return 0;
}