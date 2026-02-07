CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread -O2
LDFLAGS = -lrt -lpthread

SRC_DIR = ./src
BUILD_DIR = ./build
INCLUDE_DIR = ./include

READER_SRC = ./example_reader.cpp $(SRC_DIR)/BaseMemory.cpp
WRITER_SRC = ./example_writer.cpp $(SRC_DIR)/BaseMemory.cpp

all: reader writer

reader: $(READER_SRC)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $(READER_SRC) -o $(BUILD_DIR)/reader $(LDFLAGS)

writer: $(WRITER_SRC)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $(WRITER_SRC) -o $(BUILD_DIR)/writer $(LDFLAGS)

clean:
	rm -f $(BUILD_DIR)/reader $(BUILD_DIR)/writer

run_reader:
	./build/reader

run_writer:
	./build/writer

.PHONY: all clean reader writer