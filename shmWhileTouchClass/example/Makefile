CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread -O2
LDFLAGS = -lrt -lpthread

all: reader writer

reader: example_reader.cpp ../src/reader.cpp build/
	$(CXX) $(CXXFLAGS) example_reader.cpp  ../src/reader.cpp -o build/reader $(LDFLAGS)

writer: example_writer.cpp ../src/writer.cpp build/
	$(CXX) $(CXXFLAGS) example_writer.cpp  ../src/writer.cpp -o build/writer $(LDFLAGS)

clean:
	rm -f build/writer build/reader

run_reader:
	./build/reader

run_writer:
	./build/writer

.PHONY: all clean run_writer run_reader