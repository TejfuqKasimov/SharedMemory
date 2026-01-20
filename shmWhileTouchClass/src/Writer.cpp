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
    shm_unlink(SHM_NAME);
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

    return true;
}

void Writer::run() {
    size_t message_num = 0;
    int length_messages[3] = {128, 1024, 8192};
    while (queue->message_count <= MAX_MESSAGES) {
        // Генерируем сообщение
        std::string str = generateFixedLengthMessage(MESSAGE_SIZE - 1);

        char* message = new char[MESSAGE_SIZE];  // +1 для нуль-терминатора
        strcpy(message, str.c_str());
        
        // Отправляем
        sendMessage(message, 8192);
        
        message_num = message_num + 1;
        if (message_num % 1000 == 0) {
            std::cout << message_num << std::endl;
        }
        // Имитируем работу
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Writer::requestShutdown() {
    //queue->shutdown_requested = true;
}
