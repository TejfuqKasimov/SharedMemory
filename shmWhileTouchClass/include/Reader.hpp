#ifndef READER_H
#define READER_H

#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <thread>

#define SHM_NAME "/my_shared_queue"
#define SHM_SIZE (100 * 1024)  // 100KB
#define MAX_MESSAGES 100
#define MESSAGE_SIZE 256

struct SharedQueue {
    int write_index;
    int read_index;
    int message_count;
    bool shutdown_requested;
    char buffer[MAX_MESSAGES][MESSAGE_SIZE];
};


class Reader {
private:
    int shm_fd;
    SharedQueue* queue;
    
public:
    Reader();
    
    ~Reader();
    
    bool hasMessages();
    
    std::string getMessage();
    
    void processMessage(const std::string& message);
    
    void run();
};

#endif  // READER_H
