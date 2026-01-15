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
    // Открываем существующий сегмент разделяемой памяти
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        std::cerr << "Ошибка открытия shared memory: " << strerror(errno) << std::endl;
        return 1;
    }

    // Проецируем shared memory в адресное пространство процесса
    void* ptr = mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Ошибка mmap: " << strerror(errno) << std::endl;
        close(shm_fd);
        return 1;
    }

    // Читаем данные из shared memory
    std::cout << "Клиент читает: " << static_cast<char*>(ptr) << std::endl;

    // Освобождаем ресурсы
    munmap(ptr, SHM_SIZE);
    close(shm_fd);

    return 0;
}