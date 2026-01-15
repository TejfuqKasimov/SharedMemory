#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <csignal>

const char* SHM_NAME = "/continuous_shm";
const char* SEM_FULL = "/sem_full";
const char* SEM_EMPTY = "/sem_empty";
const char* SEM_MUTEX = "/sem_mutex";

struct SharedData {
    char buffer[10][256];
    int write_index;
    int read_index;
    bool producer_active;
};

volatile bool running = true;

void signal_handler(int) {
    running = false;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signal_handler);
    
    if (argc < 2) {
        std::cout << "Использование: " << argv[0] << " <ID потребителя>" << std::endl;
        return 1;
    }
    
    int consumer_id = atoi(argv[1]);
    std::cout << "Consumer " << consumer_id << " запущен" << std::endl;

    // Открываем семафоры
    sem_t* sem_full = sem_open(SEM_FULL, 0);
    sem_t* sem_empty = sem_open(SEM_EMPTY, 0);
    sem_t* sem_mutex = sem_open(SEM_MUTEX, 0);
    
    if (sem_full == SEM_FAILED || sem_empty == SEM_FAILED || sem_mutex == SEM_FAILED) {
        std::cerr << "Ошибка открытия семафоров. Запущен ли Producer?" << std::endl;
        return 1;
    }

    // Открываем shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Ошибка открытия shared memory: " << strerror(errno) << std::endl;
        sem_close(sem_full);
        sem_close(sem_empty);
        sem_close(sem_mutex);
        return 1;
    }
    
    SharedData* shared_data = static_cast<SharedData*>(
        mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0)
    );
    
    if (shared_data == MAP_FAILED) {
        std::cerr << "Ошибка mmap: " << strerror(errno) << std::endl;
        close(shm_fd);
        sem_close(sem_full);
        sem_close(sem_empty);
        sem_close(sem_mutex);
        return 1;
    }

    srand(static_cast<unsigned int>(consumer_id * time(nullptr)));
    int messages_received = 0;

    // Основной цикл
    while (running) {
        if (sem_trywait(sem_full) == 0) {
            sem_wait(sem_mutex);
            
            if (shared_data->read_index != shared_data->write_index) {
                std::cout << "Consumer " << consumer_id << " [" << ++messages_received 
                          << "]: " << shared_data->buffer[shared_data->read_index] << std::endl;
                
                shared_data->read_index = (shared_data->read_index + 1) % 10;
                
                sem_post(sem_mutex);
                sem_post(sem_empty);
                
                usleep(200000 + (rand() % 800000));
            } else {
                sem_post(sem_mutex);
                sem_post(sem_full);
            }
        } else {
            // Проверяем активность производителя
            sem_wait(sem_mutex);
            bool active = shared_data->producer_active;
            sem_post(sem_mutex);
            
            if (!active) {
                std::cout << "Consumer " << consumer_id << " завершает работу" << std::endl;
                break;
            }
            usleep(100000); // Короткая пауза
        }
    }

    // Очистка
    munmap(shared_data, sizeof(SharedData));
    close(shm_fd);
    sem_close(sem_full);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    return 0;
}