#ifndef WRITER_H
#define WRITER_H

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
#define MAX_MESSAGES 5
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

class Writer {
private:
    int shm_fd;
    SharedQueue* queue;
    
public:
    Writer();
    
    ~Writer();

    bool sendMessage(const char*, const int&);
    
    void run();
    
    void requestShutdown();
};

#endif  // WRITER_H
