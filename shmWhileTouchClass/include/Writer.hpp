#ifndef WRITER_H
#define WRITER_H

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

class Writer {
private:
    int shm_fd;
    SharedQueue* queue;
    
public:
    Writer();
    
    ~Writer();

    bool sendMessage(const char*);
    
    void run();
    
    void requestShutdown();
};

#endif  // WRITER_H
