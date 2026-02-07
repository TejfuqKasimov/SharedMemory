#include "../include/BaseMemory.hpp"
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <unistd.h>      // Для sleep, close
#include <fcntl.h>       // Для O_CREAT, O_RDWR
#include <sys/mman.h>    // Для mmap, munmap, PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FAILED
#include <sys/stat.h>    // Для ftruncate
#include <fcntl.h>       // Для O_CREAT, O_RDWR

// Конструктор
BaseMemory::BaseMemory(const char* name) 
    : this_shm_fd(-1), this_queue(nullptr), 
      send_shm_fd(-1), send_queue(nullptr) {
    strncpy(shm_name, name, sizeof(shm_name) - 1);
    shm_name[sizeof(shm_name) - 1] = '\0';
}

bool BaseMemory::createConnection() {
    // Создаем или открываем shared memory
    this_shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (this_shm_fd == -1) {
        perror("shm_open");
        return false;
    }
    
    // Устанавливаем размер
    if (ftruncate(this_shm_fd, SHM_SIZE) == -1) {
        close(this_shm_fd);
        shm_unlink(shm_name);
        perror("ftruncate");
        return false;
    }
    
    // Отображаем в память
    this_queue = static_cast<SharedQueue*>(
        mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, this_shm_fd, 0)
    );
    
    if (this_queue == MAP_FAILED) {
        close(this_shm_fd);
        shm_unlink(shm_name);
        perror("mmap");
        return false;
    }

    std::lock_guard<std::mutex> lock(init_mutex);
    if (!(this_queue->initialized)) {
        this_queue->write_index = 0;
        this_queue->read_index = 0;
        this_queue->message_count = 0;
        this_queue->initialized = true;
    }

    return true;
}

BaseMemory::~BaseMemory() {
    deleteConnection();
}

bool BaseMemory::sendMessage(const char* message) {
    if (!send_queue || !message) {
        return false;
    }
    
    // Проверяем, есть ли место в очереди
    if (send_queue->message_count >= MAX_MESSAGES) {
        std::cout << "Очередь переполнена, сообщение отброшено" << std::endl;
        return false;
    }
    
    // Копируем сообщение с безопасным ограничением длины
    size_t len = strlen(message);
    size_t copy_len = (len < MESSAGE_SIZE - 1) ? len : MESSAGE_SIZE - 1;
    memcpy(send_queue->buffer[send_queue->write_index], message, copy_len);
    send_queue->buffer[send_queue->write_index][copy_len] = '\0';

    // Обновляем индексы
    send_queue->write_index = (send_queue->write_index + 1) % MAX_MESSAGES;

    // Атомарно увеличиваем счетчик
    send_queue->message_count.fetch_add(1, std::memory_order_release);

    return true;
}

bool BaseMemory::openConnection(const char* name) {
    send_shm_fd = shm_open(name, O_RDWR, 0666);
    if (send_shm_fd == -1) {
        perror("shm_open");
        return false;
    }

    // Отображаем в память
    send_queue = static_cast<SharedQueue*>(
        mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, send_shm_fd, 0)
    );
    
    if (send_queue == MAP_FAILED) {
        close(send_shm_fd);
        perror("mmap");
        return false;
    }
    
    // Проверяем инициализацию
    if (!send_queue->initialized) {
        munmap(send_queue, SHM_SIZE);
        close(send_shm_fd);
        std::cerr << "Shared memory not initialized by other process" << std::endl;
        return false;
    }
    
    return true;
}

bool BaseMemory::closeConnection() {
    bool success = true;

    if (send_queue != nullptr) {
        if (munmap(send_queue, SHM_SIZE) == -1) {
            perror("munmap failed");
            success = false;
        }
        send_queue = nullptr;
    }
    
    if (send_shm_fd != -1) {
        if (close(send_shm_fd) == -1) {
            perror("close failed");
            success = false;
        }
        send_shm_fd = -1;
    }

    return success;
}

// Версия 1: запись в переданный буфер
bool BaseMemory::getMessage(char* buffer, size_t buffer_size) {
    if (!this_queue || !buffer || buffer_size < MESSAGE_SIZE) {
        return false;
    }
    
    if (this_queue->message_count.load(std::memory_order_acquire) <= 0) {
        buffer[0] = '\0';
        return false;
    }
    
    // Копируем сообщение
    memcpy(buffer, this_queue->buffer[this_queue->read_index], MESSAGE_SIZE);
    
    // Обновляем индексы
    this_queue->read_index = (this_queue->read_index + 1) % MAX_MESSAGES;
    
    // Атомарно уменьшаем счетчик
    this_queue->message_count.fetch_sub(1, std::memory_order_release);
    
    return true;
}

// Версия 2: возвращает std::string (для обратной совместимости)
std::string BaseMemory::getMessage() {
    char buffer[MESSAGE_SIZE];
    if (getMessage(buffer, sizeof(buffer))) {
        return std::string(buffer);
    }
    return "";  // Возвращаем пустую строку вместо nullptr
}

bool BaseMemory::deleteConnection() {
    bool success = true;
    
    // Освобождаем свою очередь
    if (this_queue != nullptr) {
        if (munmap(this_queue, SHM_SIZE) == -1) {
            perror("munmap failed");
            success = false;
        }
        this_queue = nullptr;
    }
    
    if (this_shm_fd != -1) {
        if (close(this_shm_fd) == -1) {
            perror("close failed");
            success = false;
        }
        this_shm_fd = -1;
    }
    
    // Удаляем разделяемую память
    if (shm_unlink(shm_name) == -1) {
        perror("shm_unlink failed");
        success = false;
    }
    
    // Также закрываем соединение для отправки, если оно открыто
    closeConnection();
    
    return success;
}

bool BaseMemory::hasMessage() {
    if (!this_queue) {
        return false;
    }
    return this_queue->message_count.load(std::memory_order_acquire) > 0;
}