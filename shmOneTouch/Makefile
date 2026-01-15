CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread

all: writer reader

writer: writer.cpp
	$(CXX) $(CXXFLAGS) -o writer writer.cpp -lrt

reader: reader.cpp
	$(CXX) $(CXXFLAGS) -o reader reader.cpp -lrt

clean:
	rm -f writer reader

.PHONY: all clean