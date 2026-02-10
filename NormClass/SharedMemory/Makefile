CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pthread -O2
LDFLAGS = -lrt -lpthread

SRC_DIR = ./src
BUILD_DIR = ./build
INCLUDE_DIR = ./include

READER_SRC = ./example_reader.cpp $(SRC_DIR)/BaseMemory.cpp
WRITER1_SRC = ./example_writer.cpp $(SRC_DIR)/BaseMemory.cpp
WRITER2_SRC = ./example_writer2.cpp $(SRC_DIR)/BaseMemory.cpp

all: reader writer1 writer2

reader: $(READER_SRC)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $(READER_SRC) -o $(BUILD_DIR)/reader $(LDFLAGS)

writer1: $(WRITER1_SRC)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $(WRITER1_SRC) -o $(BUILD_DIR)/writer1 $(LDFLAGS)

writer2: $(WRITER2_SRC)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $(WRITER2_SRC) -o $(BUILD_DIR)/writer2 $(LDFLAGS)

clean:
	rm -f $(BUILD_DIR)/reader $(BUILD_DIR)/writer1 $(BUILD_DIR)/writer2

run_reader:
	./build/reader

run_writer1:
	./build/writer1

run_writer2:
	./build/writer2

.PHONY: all clean reader writer