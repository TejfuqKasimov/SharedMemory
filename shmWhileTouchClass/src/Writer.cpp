#include "../include/Writer.hpp"
#include "../include/WordGenerator.hpp"

Writer::Writer() {
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

Writer::~Writer() {
    munmap(queue, SHM_SIZE);
    close(shm_fd);
}

bool Writer::sendMessage(const char* message, const int& size) {
    // Проверяем, есть ли место в очереди
    if (queue->message_count >= MAX_MESSAGES) {
        std::cout << "Очередь переполнена, сообщение отброшено" << std::endl;
        return false;
    }
    
    // Копируем сообщение
    strncpy(queue->buffer[queue->write_index], message, MESSAGE_SIZE - 1);
    queue->buffer[queue->write_index][MESSAGE_SIZE - 1] = '\0';
    
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    // Добавляем время отправки
    queue->time_send[queue->write_index] = now;

    queue->size[queue->write_index] = size;

    // Обновляем индексы
    queue->write_index = (queue->write_index + 1) % MAX_MESSAGES;

    // Атомарно увеличиваем счетчик
    __sync_fetch_and_add(&queue->message_count, 1);
    // std::cout << "Сообщение: " << message << std::endl;
    std::cout << "Отправлено " << size << " bytes"
                << " (в очереди: " << queue->message_count << ")" << std::endl;
    return true;
}

void Writer::run() {
    size_t message_num = 0;
    int length_messages[3] = {128, 1024, 8192};
    while (!queue->shutdown_requested) {
        // Генерируем сообщение
        std::string str = generateFixedLengthMessage(length_messages[message_num]);

        char* message = new char[MESSAGE_SIZE];  // +1 для нуль-терминатора
        strcpy(message, str.c_str());
        
        // Отправляем
        sendMessage(message, length_messages[message_num]);
        
        message_num = (message_num + 1) % 3;

        // Имитируем работу
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Writer::requestShutdown() {
    //queue->shutdown_requested = true;
}
