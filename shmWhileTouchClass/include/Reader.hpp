#ifndef READER_H
#define READER_H

#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <chrono>

#define SHM_NAME "/my_shared_queue"
#define SHM_SIZE (100 * 2048)  // 200KB
#define MAX_MESSAGES 20
#define MESSAGE_SIZE 8200

struct SharedQueue {
    char buffer[MAX_MESSAGES][MESSAGE_SIZE];
    std::chrono::steady_clock::time_point time_send[MAX_MESSAGES];
    int size[MAX_MESSAGES];
    int write_index;
    int read_index;
    int message_count;
    bool shutdown_requested;
};

struct OneMessage {
    int size;
    std::string message;
    std::chrono::steady_clock::time_point time_recieved;
};

class Reader {
private:
    int shm_fd;
    SharedQueue* queue;
    
public:
    Reader();
    
    bool isShutdownRequested();

    ~Reader();
    
    bool hasMessages();
    
    OneMessage getMessage();
    void processMessage(const std::string&, 
                        const std::chrono::steady_clock::time_point&,
                        const int&);
    
    void run();
};

#endif  // READER_H
