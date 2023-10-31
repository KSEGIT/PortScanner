// #PortScanner
// Daniel Kiska
// 26.10.2023

#include "portscan.hpp"

void count_openPorts(int start, int end) {
    
    // Data structure for port arguments
    int sockfd;
    struct sockaddr_in tower;
    
    // Making sure that port range is valid
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

    // Prepare addrinfo hints
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;

    // Resolve the hostname to an IP address
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

        for (int portNum = start; portNum <= end; portNum++) {
            tower.sin_port = htons(portNum);

            try {
                if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                    throw std::runtime_error("Failed to create socket.");
                }

                if (connect(sockfd, (struct sockaddr*)&tower, sizeof(tower)) == 0) {
                    {
                        // TODO further resarch on thread-safe container like 
                        // std::vector<std::atomic<int>> to avoid locking overhead 
                        //when accessing the vector in a read-only context from multiple threads.
                        std::lock_guard<std::mutex> lock(vecMutex);
                        openPorts.push_back(portNum);
                    }
                }
                close(sockfd); // Close the socket when it's successfully created
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
                // Optionally, continue or break the loop depending on requirements.
                continue;
            }
        }
    }

    // Free the addrinfo structure
    freeaddrinfo(result);
}

// For printing verbose info
void verbose_printer(char flag){
    std::cout << "\033[1;34m===== Port Scan Finished - with flag: " << (char)flag << " =====\033[0m\n";
    return;
}

// Preparing ip address for banner scanner (adding port)
const char* ip_and_port (const char* g_ipAddress ,const char* portNumber){

    // Convert the C-style strings to C++ strings
    std::string ipAddressString(g_ipAddress);
    std::string additionalString(portNumber);

    // Concatenate the C++ strings
    std::string combinedString = ipAddressString + additionalString;

    // Convert the combined string back to a const char*
    const char* result = combinedString.c_str();

    // Verbose ip combination print
    if (g_verbose){
    std::cout << "Combined IP Address: " << result << std::endl;
    }

    return result;
}

// Printing open ports
void print_ports(std::vector<int>& openPorts, int start, int end, char flag){

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

    // TODO: Possible point of optimization:
    // For loop or std::find runs in O(n) time, 
    // but std::set has its own find() member (ie. myset.find(x)) which runs in O(log n) time - 
    // that's much more efficient with large numbers of elements - consider using sets instead of vector
    /*      if(std::find(openPorts.begin(), openPorts.end(), 80) != openPorts.end()) {
                if (g_verbose){
                std::cout << "\033[1;34m===== Starting Banner Scan =====\033[0m\n";
                }
                getBanner(ipAddress);
            }   */
    if (openPorts.empty()) { 
        std::cout << "No open ports found, trying to get any host response..." << std::endl;
        getBanner(g_ipAddress);
    }
    else{
        for (int num : openPorts) {
            if (num == 80) {
                std::cout << "\033[1;34m===== Starting Banner Scan for port 80 =====\033[0m\n";
                const char * ipAddress = ip_and_port(g_ipAddress, ":80");
                getBanner(ipAddress);
            } else if (num == 443) {
                std::cout << "\033[1;34m===== Starting Banner Scan for port 443 =====\033[0m\n";
                const char * ipAddress = ip_and_port(g_ipAddress, ":443");
                getBanner(ipAddress);
            } else if (num == 445) {
                std::cout << "\033[1;34m===== Starting Banner Scan for port 445 =====\033[0m\n";
                std::cout << "TODO: implement samba comunication\n";
            }
        } 
    }   
}

// Multithreading handler
void thread_handler(int start, int end, char flag){

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

    std::cout << "Starting range port scan...\n";
    // Thread loop
    for (unsigned int thread_num = 0; thread_num < maxThreads; ++thread_num) {
        int rightBound = start + intervalSize;
        thread_list.push_back(std::thread(count_openPorts, start, rightBound));
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
	print_ports(openPorts, start, end, flag);

    // TODO: Add additional verbose data
    if (g_verbose){
        verbose_printer(flag);
    }
}
