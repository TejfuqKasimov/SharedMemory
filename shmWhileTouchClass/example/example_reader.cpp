#include "../include/Reader.hpp"
#include <vector>
#include <algorithm>

int main() {
    Reader reader;
    std::vector <int> Benchmark;
    for (int i = 0; i < 100; ++i) {
        OneMessage message1 = reader.getMessage();
    }
    
    std::cout << "sleep message is over" << std::endl;
    
    while (reader.isShutdownRequested() && Benchmark.size() < 100000) {
        if (reader.hasMessages()) {
            OneMessage message = reader.getMessage();

            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

            std::chrono::milliseconds duration_milli = 
                std::chrono::duration_cast<std::chrono::milliseconds>(now - message.time_recieved);
            std::chrono::nanoseconds duration_nano = 
                std::chrono::duration_cast<std::chrono::nanoseconds>(now - message.time_recieved);
            
            // std::cout << "Читатель получил сообщение " << message.size << " bytes" << std::endl;
            // std::cout << "Прошло " << duration_milli.count() << " милиcекунд" << std::endl;
            // std::cout << "Или " << duration_nano.count() << " наносекунд" << std::endl;
            Benchmark.push_back(duration_nano.count());
            // if (Benchmark.size() % 100 == 0)
            //     std::cout << "Benchmark size is " << Benchmark.size() << std::endl;
        }
    }

    std::sort(Benchmark.begin(), Benchmark.end());

    // for (int i : Benchmark) {
    //     std::cout << i << " ";
    // }
    std::cout << std::endl;
    std::cout << "Benchmark for ... is " << Benchmark[(Benchmark.size() - 1) / 2] << std::endl;

    return 0;
}