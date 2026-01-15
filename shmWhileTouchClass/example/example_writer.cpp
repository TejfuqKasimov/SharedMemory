#include "../include/Writer.hpp"

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