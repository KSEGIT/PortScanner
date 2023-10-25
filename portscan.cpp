

#include "portscan.hpp"

void count_openPorts(int start, int end){
    int sockfd;
    struct sockaddr_in tower;

    //localhost is 127.0.0.1 (hopefully)
    /* if (inet_pton(AF_INET, "127.0.0.1", &tower.sin_addr) < 1){
        fprintf(stderr, "Problem loading your IP address\n");
        exit(EXIT_FAILURE);
    } */
    memset(&tower, 0, sizeof(tower));
    tower.sin_family = AF_INET;
    tower.sin_addr.s_addr = inet_addr(ipAddress);

    for (int port_num = start; port_num <= end; port_num++){
        tower.sin_port = htons(port_num);
        if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
            fprintf(stderr, "Error: Failed to create socket.\nPlease try again\n");
            close(sockfd);
            port_num--;
            continue;
        }
        if (connect(sockfd, (struct sockaddr*) &tower, sizeof(tower)) == 0){
            lock_guard<std::mutex> guard(vecMutex);
            openPorts.push_back(port_num);
        }
        close(sockfd);
    }
}

//For printing verbose info
void verbose_printer(char flag){
    cout << "\n-verbose print debug\n\n";//debug
    return;
}

//Printing open ports
void print_ports(std::vector<int>& openPorts, int start, int end, char flag){
	switch(flag){
        case 's':
            std::cout << "\033[1;34m===== Open System Ports =====\033[0m\n";
            for (auto&& item : openPorts){
                std::cout << "\033[1m" << item << "\033[0m";
                if (portMap.find(item) != portMap.end()){
                    std::cout << " -> " << portMap[item] << "\n";   
                }
                else{
                    std::cout << "\n";
                }
			}
        	break;

        default:
            std::cout << "\033[1;34m===== Open Ports =====\033[0m\n";
            for (int i : openPorts){
            std::cout << "\033[1m" << i << "\033[0m\n";
            }
            std::cout << "\033[1;34m===== Open Ports =====\033[0m\n";
	}   
}

//Multithreading handler
void thread_handler(const char * ipAddress, int start, int end, char flag){
    //Threads configuration
    int max_threads = thread::hardware_concurrency();
    thread thread_list[max_threads];
    int interval_size = (end - start + 1)/max_threads;
    int thread_num;

    //create all the threads
    for (thread_num = 0; thread_num < max_threads; thread_num++){
        int right_bound = start + interval_size;
        thread_list[thread_num] = thread(count_openPorts, start, right_bound);
        start = right_bound + 1;
    }
    //round up all the threads
    for (thread_num = 0; thread_num < max_threads; thread_num++){
        thread_list[thread_num].join();
    }

    //Sorting list of open ports
	sort(openPorts.begin(), openPorts.end());

    //Printing open ports to console
	print_ports(openPorts, start, end, flag);

    //TODO: Add verbose data
    if (verbose){
        verbose_printer(flag);
    }
    
}
