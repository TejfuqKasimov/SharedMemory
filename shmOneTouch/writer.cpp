#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

const char* SHM_NAME = "/my_shared_memory";
const size_t SHM_SIZE = 4096; // 4KB

int main() {
    // Создаем или открываем сегмент разделяемой памяти
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Ошибка создания shared memory: " << strerror(errno) << std::endl;
        return 1;
    }

    // Устанавливаем размер сегмента
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        std::cerr << "Ошибка установки размера: " << strerror(errno) << std::endl;
        shm_unlink(SHM_NAME);
        return 1;
    }

    // Проецируем shared memory в адресное пространство процесса
    void* ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Ошибка mmap: " << strerror(errno) << std::endl;
        shm_unlink(SHM_NAME);
        return 1;
    }

    // Записываем данные в shared memory
    const char* message = "Привет из shared memory!";
    std::cout << "Сервер записывает: " << message << std::endl;
    
    // Используем snprintf для безопасной записи
    snprintf(static_cast<char*>(ptr), SHM_SIZE, "%s", message);

    // Ждем нажатия Enter перед завершением
    std::cout << "Данные записаны. Нажмите Enter для завершения..." << std::endl;
    std::cin.get();

    // Освобождаем ресурсы
    munmap(ptr, SHM_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME); // Удаляем сегмент

    return 0;
}