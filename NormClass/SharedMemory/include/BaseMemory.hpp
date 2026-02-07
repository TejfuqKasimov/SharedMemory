#ifndef BASE_MEMORY_HPP
#define BASE_MEMORY_HPP

#include <atomic>
#include <mutex>
#include <cstddef>
#include <string>

#define MAX_MESSAGES 10
#define MESSAGE_SIZE 8192
#define SHM_SIZE (sizeof(SharedQueue))

struct SharedQueue {
    char buffer[MAX_MESSAGES][MESSAGE_SIZE];
    size_t write_index;
    size_t read_index;
    std::atomic<int> message_count;
    bool initialized;
};

class BaseMemory {
private:
    char shm_name[256];
    int this_shm_fd;
    SharedQueue* this_queue;
    
    int send_shm_fd;
    SharedQueue* send_queue;
    
    std::mutex init_mutex;

public:
    BaseMemory(const char* name);
    ~BaseMemory();
    
    bool createConnection();
    bool deleteConnection();
    bool openConnection(const char* name);
    bool closeConnection();
    
    bool sendMessage(const char* message);
    
    // Два варианта getMessage:
    bool getMessage(char* buffer, size_t buffer_size);  // Версия 1: в переданный буфер
    std::string getMessage();                           // Версия 2: возвращает std::string
    
    bool hasMessage();
};

#endif