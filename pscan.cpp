/* a multithreaded C program to check for open ports on Linux machines. 
   i think it should also work on macOS and BSD systems if compiled with g++ for the thread library */

#include "pscan.hpp"

void count_open_ports(int start, int end){
    int sockfd;
    struct sockaddr_in tower;

    //localhost is 127.0.0.1 (hopefully)
    if (inet_pton(AF_INET, "127.0.0.1", &tower.sin_addr) < 1){
        fprintf(stderr, "Problem loading your IP address\n");
        exit(EXIT_FAILURE);
    }
    memset(&tower, 0, sizeof(tower));
    tower.sin_family = AF_INET;
    tower.sin_addr.s_addr = inet_addr("192.168.1.1");

    for (int port_num = start; port_num <= end; port_num++){
        tower.sin_port = htons(port_num);
        if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
            fprintf(stderr, "Error: Failed to create socket.\nPlease try again\n");
            close(sockfd);
            port_num--;
            continue;
        }
        if (connect(sockfd, (struct sockaddr*) &tower, sizeof(tower)) == 0){
            //cout << std::setw(11);//debug
            lock_guard<mutex> guard(vec_mtx);
            open_ports.push_back(port_num);
        }
        close(sockfd);
    }
}

void verbose_printer(char flag){
    std::cout << "";
    return;
}

void print_ports(vector<int>& open_ports, int start, int end, char flag){
	switch(flag){
        case 's':
            cout << "\033[1;34m===== Open System Ports =====\033[0m\n";
            for (int i : open_ports){
                cout << "\033[1m" << i << "\033[0m";
                if (port_map.find(i) != port_map.end()){
                    cout << " -> " << port_map[i] << "\n";   
                }
                else{
                    cout << "\n";
                }
			}
        	break;

        default:
            for (int i : open_ports){
                cout << "\033[1m" << i << "\033[0m\n";
            }
	}   
}

void thread_handler(int start, int end, char flag){
    int max_threads = thread::hardware_concurrency();
    thread thread_list[max_threads];
    int interval_size = (end - start + 1)/max_threads;
    int thread_num;

    //create all the threads
    for (thread_num = 0; thread_num < max_threads; thread_num++){
        int right_bound = start + interval_size;
        thread_list[thread_num] = thread(count_open_ports, start, right_bound);
        start = right_bound + 1;
    }
    //round up all the threads
    for (thread_num = 0; thread_num < max_threads; thread_num++){
        thread_list[thread_num].join();
    }

	sort(open_ports.begin(), open_ports.end());

	print_ports(open_ports, start, end, flag);

    if (verbose){
        verbose_printer(flag);
    }
    
}
