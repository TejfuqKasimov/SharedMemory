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
    Writer() {
        // Создаем или открываем shared memory
        shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_fd == -1) {
            perror("shm_open");
            exit(1);
        }
        
        // Устанавливаем размер
        if (ftruncate(shm_fd, SHM_SIZE) == -1) {
            shm_unlink(SHM_NAME);
            perror("shm_open");
            exit(1);
        }
        
        // Отображаем в память
        queue = static_cast<SharedQueue*>(
            mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0)
        );
        
        if (queue == MAP_FAILED) {
            shm_unlink(SHM_NAME);
            perror("mmap");
            exit(1);
        }
        
        // Инициализируем очередь (если мы первые)
        static bool initialized = false;
        if (!initialized) {
            queue->write_index = 0;
            queue->read_index = 0;
            queue->message_count = 0;
            queue->shutdown_requested = false;
            initialized = true;
        }
    }
    
    ~Writer() {
        munmap(queue, SHM_SIZE);
        close(shm_fd);
    }
    
    bool sendMessage(const char* message) {
        // Проверяем, есть ли место в очереди
        if (queue->message_count >= MAX_MESSAGES) {
            std::cout << "Очередь переполнена, сообщение отброшено: " 
                      << message << std::endl;
            return false;
        }
        
        // Копируем сообщение
        strncpy(queue->buffer[queue->write_index], message, MESSAGE_SIZE - 1);
        queue->buffer[queue->write_index][MESSAGE_SIZE - 1] = '\0';
        
        // Обновляем индексы
        queue->write_index = (queue->write_index + 1) % MAX_MESSAGES;
        
        // Атомарно увеличиваем счетчик
        __sync_fetch_and_add(&queue->message_count, 1);
        
        std::cout << "Отправлено: " << message 
                  << " (в очереди: " << queue->message_count << ")" << std::endl;
        return true;
    }
    
    void run() {
        int message_num = 1;
        while (!queue->shutdown_requested) {
            // Генерируем сообщение
            char message[256];
            snprintf(message, sizeof(message), 
                    "Сообщение #%d от писателя", message_num++);
            
            // Отправляем
            sendMessage(message);
            
            // Имитируем работу
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    
    void requestShutdown() {
        queue->shutdown_requested = true;
    }
};

int main() {
    Writer writer;
    
    // Запускаем в отдельном потоке для демонстрации
    std::thread writer_thread([&writer]() {
        writer.run();
    });
    
    // Даем поработать 30 секунд
    std::this_thread::sleep_for(std::chrono::seconds(30));
    
    writer.requestShutdown();
    writer_thread.join();
    
    // Очистка
    shm_unlink(SHM_NAME);
    
    return 0;
}