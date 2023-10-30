
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

    // Setting memory buffer
    memset(&tower, 0, sizeof(tower));
    tower.sin_family = AF_INET;
    tower.sin_addr.s_addr = inet_addr(ipAddress);

    for (int portNum = start; portNum <= end; portNum++) {
        tower.sin_port = htons(portNum);

        try {
            if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                throw std::runtime_error("Failed to create socket.");
            }

            if (connect(sockfd, (struct sockaddr*)&tower, sizeof(tower)) == 0) {
                {
                    std::lock_guard<std::mutex> guard(vecMutex);
                    openPorts.push_back(portNum);
                }
            }

            close(sockfd); // Close the socket when it's successfully created
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            // Optionally, you can continue or break the loop depending on your requirements.
            continue;
        }
    }
}

// For printing verbose info
void verbose_printer(char flag){
    std::cout << "\033[1;34m===== Port Scan Finished =====\033[0m\n";
    return;
}

// Printing open ports
void print_ports(std::vector<int>& openPorts, int start, int end, char flag){

	// Different printing can be used depending on provided flag
    switch(flag){
        case 's':
            std::cout << "\033[1;34m===== Open System Ports =====\033[0m\n";
            for (auto&& item : openPorts){
            std::cout << "\033[1m" << item << "\033[0m\n";
            }
            std::cout << "\n";
        	break;

        default:
            std::cout << "\033[1;34m===== Open Ports =====\033[0m\n";
            for (auto&& item : openPorts){
            std::cout << "\033[1m" << item << "\033[0m\n";
            }
            std::cout << "\n";
	}

    // TODO: Possible point of optimalization:
    // std::find(vec.begin(), vec.end(), x) runs in O(n) time, 
    // but std::set has its own find() member (ie. myset.find(x)) which runs in O(log n) time - 
    // that's much more efficient with large numbers of elements
    if(std::find(openPorts.begin(), openPorts.end(), 80) != openPorts.end()) {
       if (g_verbose){
       std::cout << "\033[1;34m===== Starting Banner Scan =====\033[0m\n";
       }
       getBanner(ipAddress);
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
    int interval_size = (end - start) / maxThreads;
    
    // List of threads
    std::vector<std::thread> thread_list;

    // Thread loop
    for (unsigned int thread_num = 0; thread_num < maxThreads; ++thread_num) {
        int right_bound = start + interval_size;
        thread_list.push_back(std::thread(count_openPorts, start, right_bound));
        start = right_bound + 1;
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
