#include "../include/Reader.hpp"

Reader::Reader() {
    // Открываем существующую shared memory
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
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
}
    
Reader::~Reader() {
    munmap(queue, SHM_SIZE);
    close(shm_fd);
}

bool Reader::hasMessages() {
    return queue->message_count > 0;
}

std::string Reader::getMessage() {
    if (queue->message_count <= 0) {
        return "";
    }
    
    // Копируем сообщение
    char message[MESSAGE_SIZE];
    strncpy(message, queue->buffer[queue->read_index], MESSAGE_SIZE);
    
    // Обновляем индексы
    queue->read_index = (queue->read_index + 1) % MAX_MESSAGES;
    
    // Атомарно уменьшаем счетчик
    __sync_fetch_and_sub(&queue->message_count, 1);
    
    return std::string(message);
}

void Reader::processMessage(const std::string& message) {
    // Имитация обработки
    std::cout << "Читатель обрабатывает: " << message << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Обработка завершена: " << message << std::endl;
}

void Reader::run() {
    while (!queue->shutdown_requested) {
        if (hasMessages()) {
            std::string message = getMessage();
            if (!message.empty()) {
                processMessage(message);
            }
        } else {
            // Нет сообщений - небольшая пауза
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}