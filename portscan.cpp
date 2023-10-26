
#include "portscan.hpp"

void count_openPorts(int start, int end){
    int sockfd;
    struct sockaddr_in tower;

    // Setting memory buffer
    memset(&tower, 0, sizeof(tower));
    tower.sin_family = AF_INET;
    tower.sin_addr.s_addr = inet_addr(ipAddress);

    for (int portNum = start; portNum <= end; portNum++){
        tower.sin_port = htons(portNum);
        if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
            fprintf(stderr, "Error: Failed to create socket.\nPlease try again\n");
            close(sockfd);
            portNum--;
            continue;
        }
        if (connect(sockfd, (struct sockaddr*) &tower, sizeof(tower)) == 0){
            lock_guard<std::mutex> guard(vecMutex);
            openPorts.push_back(portNum);
        }
        close(sockfd);
    }
}

// For printing verbose info
void verbose_printer(char flag){
    //cout << "\n-verbose print debug\n\n";//debug
    return;
}

// Printing open ports
void print_ports(std::vector<int>& openPorts, int start, int end, char flag){
    for (int port : openPorts) {
        if (port == 80 || 443 || 445) {
            //startBanner();
            //getBanner(ipAddress);
        }
    }
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
}

// Multithreading handler
void thread_handler(int start, int end, char flag){

    // Threads configuration
    int max_threads = thread::hardware_concurrency();
    thread thread_list[max_threads];
    int interval_size = (end - start + 1)/max_threads;
    int thread_num;

    // create all the threads
    for (thread_num = 0; thread_num < max_threads; thread_num++){
        int right_bound = start + interval_size;
        thread_list[thread_num] = thread(count_openPorts, start, right_bound);
        start = right_bound + 1;
    }

    // round up all the threads
    for (thread_num = 0; thread_num < max_threads; thread_num++){
        thread_list[thread_num].join();
    }

    // Sorting list of open ports
	sort(openPorts.begin(), openPorts.end());

    // Printing open ports to console
	print_ports(openPorts, start, end, flag);

    // TODO: Add verbose data
    if (g_verbose){
        verbose_printer(flag);
    }
}
