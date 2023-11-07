// #PortScanner
// Daniel Kiska
// 26.10.2023

#include "portscan.hpp"


void scanRangeOpenPorts(int start, int end) {
    // Data structure for port arguments
    int sockfd;
    struct sockaddr_in tower;
    // Setting memory buffer
    memset(&tower, 0, sizeof(tower));
    tower.sin_family = AF_INET;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    
    // Port Range validation
    if (0 > start || end > 65535){
        start = 0;
        end = 65535;
        cout << "\nPort range Error\n";
    }

    if (g_verbose){
        // Threading debug
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Thread started open port search with range: " << start << " : " << end << std::endl;
    }

    struct addrinfo *result;
    int status = getaddrinfo(g_ipAddress, NULL, &hints, &result);
    if (status != 0) {
        cerr << "Failed to resolve hostname: " << g_ipAddress << endl;
        return;
    }

    // Loop through the resolved IP addresses
    for (struct addrinfo *addr = result; addr != NULL; addr = addr->ai_next) {
        tower.sin_addr = ((struct sockaddr_in*)(addr->ai_addr))->sin_addr;
        for (int portNum = start; portNum <= end; portNum++) {
            tower.sin_port = htons(portNum);
            // Threading debug
            //if (g_verbose){std::cout << " . ";};
            try {
                if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                    throw std::runtime_error("Failed to create socket.");
                }
                if (connect(sockfd, (struct sockaddr*)&tower, sizeof(tower)) == 0) {
                    {
                        // Threading debug
                        //if (g_verbose){std::cout << " " << portNum << " ";};
                        // Add open port to openPorts vector
                        std::lock_guard<std::mutex> lock(vecMutex);
                        openPorts.push_back(portNum);
                    }
                }
                // Close the socket
                close(sockfd); 
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
                continue;
            }
        }
    }
    // Free the addrinfo structure
    freeaddrinfo(result);
}

// For printing verbose info
void verbosePrinter(char flag){
    std::cout << "\033[1;34m===== Port Scan Finished - with flag: " << (char)flag << " =====\033[0m\n";
    return;
}

// Preparing ip address for banner scanner (adding port)
const char* combineIPandPort (const char* ipAddress ,const char* portNumber){
    // Convert the C-style strings to C++ strings
    std::string ipAddressString(ipAddress);
    std::string additionalString(portNumber);
    // Concatenate the C++ strings
    std::string combinedString = ipAddressString + additionalString;
    // Convert the combined string back to a const char*
    const char* result = combinedString.c_str();
    // Verbose ip combination print
    if (g_verbose){
    std::cout << "Combined IP Address: " << result << std::endl;
    }
    // Returns const char containing ip:port
    return result;
}

const char* ensureHttpsScheme(const char* url) {
    const char* httpsPrefix = "https://";
    const char* httpPrefix = "http://";

    // Check if the URL starts with "http://" and replace it with "https://"
    if (std::strncmp(url, httpPrefix, 7) == 0) {
        size_t len = std::strlen(url) - 3;  // Remove "http://"
        char* modifiedUrl = (char*)malloc(len + 9);  // Allocate memory
        std::strcpy(modifiedUrl, "https://");
        std::strcat(modifiedUrl, url + 7);  // Copy the rest of the URL
        return modifiedUrl;
    }

    // If "http://" is not found, add "https://"
    if (std::strncmp(url, httpsPrefix, 8) != 0) {
        size_t len = std::strlen(url);
        char* modifiedUrl = (char*)malloc(len + 9);  // Allocate memory
        std::strcpy(modifiedUrl, "https://");
        std::strcat(modifiedUrl, url);  // Copy the URL as is
        return modifiedUrl;
    }

    if (g_verbose){
    std::cout << "Combined IP Address: " << url << std::endl;
    }
    return url;
}

void checkOpenPortsSSL (std::vector<int>& openPorts, char flag){
    // TODO: Possible point of optimization:
    // For loop or std::find runs in O(n) time, 
    // but std::set has its own find() member (ie. myset.find(x)) which runs in O(log n) time - 
    // that's much more efficient with large numbers of elements - consider using sets instead of vector
    if (openPorts.empty()) { 
        std::cout << "check flags configuration: "<< (char)flag << std::endl;
        std::cout << "No open ports found, trying to get any host response..." << std::endl;
        getBanner(g_ipAddress);
    }
    else{
        for (int num : openPorts) {
            if (num == 80) {
                std::cout << "\033[1;34m===== Starting Banner Scan for port 80 =====\033[0m\n";
                //const char * ipAddress = combineIPandPort(g_ipAddress, ":80");
                getBanner(g_ipAddress);
            } else if (num == 443) {
                std::cout << "\033[1;34m===== Starting Banner Scan for port 443 =====\033[0m\n";
                //const char * ipAddress = combineIPandPort(g_ipAddress, ":443");
                //const char * httpsIpAddress = combineIPandPort(ensureHttpsScheme(g_ipAddress), ":443");
                const char * httpsIpAddress = ensureHttpsScheme(g_ipAddress);
                getBanner(httpsIpAddress);
            } else if (num == 445) {
                std::cout << "\033[1;34m===== Starting Banner Scan for port 445 =====\033[0m\n";
                std::cout << "TODO: Implement samba server communication\n";
            }
        } 
    }   
}

// Printing open ports
void printPorts(std::vector<int>& openPorts, char flag){
	// Different printing can be used depending on the provided flag
    switch(flag){
        case 's':
            std::cout << "\033[1;34m===== Open System Ports =====\033[0m\n";
            for (auto&& item : openPorts){
            std::cout << "\033[1m" << item << "\033[0m\n";
            }
            std::cout << "\033[1;34m===== END System Open Ports =====\033[0m\n";
        	break;

        default:
            std::cout << "\033[1;34m===== Open Ports =====\033[0m\n";
            for (auto&& item : openPorts){
            std::cout << "\033[1m" << item << "\033[0m\n";
            }
            std::cout << "\033[1;34m===== END Open Ports =====\033[0m\n";
	}
    // Run the check for ssl certificates
    checkOpenPortsSSL(openPorts, flag);
}

// Multithreading handler
void threadHandler(int start, int end, char flag){

    if (g_verbose){
    // debug threads
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";
    }

    // Getting max_threads number
    unsigned int maxThreads = std::thread::hardware_concurrency();
    // Adjust this value as needed
    int intervalSize = (end - start) / maxThreads;
    // List of threads
    std::vector<std::thread> thread_list;

    // Thread scanning loop
    std::cout << "Starting range port scan...\n";
    for (unsigned int thread_num = 0; thread_num < maxThreads; ++thread_num) {
        int rightBound = start + intervalSize;
        thread_list.push_back(std::thread(scanRangeOpenPorts, start, rightBound));
        start = rightBound + 1;
        if (g_verbose){
        std::cout << "Thread " << thread_num << " ID: " << thread_list[thread_num].get_id() << std::endl;
        }
    }

    // Finishing threads
    for (unsigned int thread_num = 0; thread_num < maxThreads; ++thread_num) {
        thread_list[thread_num].join(); // Wait for each thread to finish
        if (g_verbose){
        std::cout << "Closing Thread " << thread_num << " ID: " << thread_list[thread_num].get_id() << std::endl;
        }
    }

    if (g_verbose){
    std::cout << "All threads have finished." << std::endl;
    }

    // Sorting list of open ports
	sort(openPorts.begin(), openPorts.end());

    // Printing open ports to console
	printPorts(openPorts, flag);

    // Add additional verbose data
    if (g_verbose){
        verbosePrinter(flag);
    }
}


void scanSingleOpenPort(int portToScan, char flag) {
    int sockfd;
    struct sockaddr_in tower;
    memset(&tower, 0, sizeof(tower));
    tower.sin_family = AF_INET;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    
    if (g_verbose){
        std::cout << "Scanning open port: " << portToScan << " using flag: " << (char)flag << std::endl;
    }
    
    struct addrinfo *result;
    int status = getaddrinfo(g_ipAddress, NULL, &hints, &result);
    if (status != 0) {
        cerr << "Failed to resolve hostname: " << g_ipAddress << endl;
        return;
    }
    
    // Loop through the resolved IP addresses
    for (struct addrinfo *addr = result; addr != NULL; addr = addr->ai_next) {
        tower.sin_family = AF_INET;
        tower.sin_addr = ((struct sockaddr_in*)(addr->ai_addr))->sin_addr;
        tower.sin_port = htons(portToScan);
        
        try {
            if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                throw std::runtime_error("Failed to create socket.");
            }
            
            if (connect(sockfd, (struct sockaddr*)&tower, sizeof(tower)) == 0) {
                {
                    openPorts.push_back(portToScan);
                }
            }
            // Close the socket
            close(sockfd);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            continue;
        }
    }
    // Free the addrinfo structure
    freeaddrinfo(result);
}

void runPrintPort (char flag){
    // Printing open ports to console
	printPorts(openPorts, flag);
}