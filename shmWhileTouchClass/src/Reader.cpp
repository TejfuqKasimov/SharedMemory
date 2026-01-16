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

OneMessage Reader::getMessage() {
    OneMessage res;
    if (queue->message_count <= 0) {
        return res;
    }
    
    // Копируем сообщение
    char message[MESSAGE_SIZE];
    strncpy(message, queue->buffer[queue->read_index], MESSAGE_SIZE);
    res.message = std::string(message);
    res.time_recieved = queue->time_send[queue->read_index];
    res.size = queue->size[queue->read_index];
    // Обновляем индексы
    queue->read_index = (queue->read_index + 1) % MAX_MESSAGES;
    
    // Атомарно уменьшаем счетчик
    __sync_fetch_and_sub(&queue->message_count, 1);
    
    return res;
}

void Reader::processMessage(const std::string& message, 
                            const std::chrono::steady_clock::time_point& receiveTime,
                            const int& size) {
    // Имитация обработки
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    std::chrono::milliseconds duration = 
        std::chrono::duration_cast<std::chrono::milliseconds>(now - receiveTime);
    
    std::cout << "Читатель получил сообщение " << size << " bytes" << std::endl;
    std::cout << "Прошло " << duration.count() << " милиcекунд" << std::endl;
    // std::cout << "Сообщение " << message << std::endl;
}

void Reader::run() {
    while (!queue->shutdown_requested) {
        if (hasMessages()) {
            OneMessage message = getMessage();
            if (!message.message.empty()) {
                processMessage(message.message, message.time_recieved, message.size);
            }
        } else {
            // Нет сообщений - небольшая пауза
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}