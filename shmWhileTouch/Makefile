CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread -O2
LDFLAGS = -lrt -lpthread

all: producer consumer multi_consumer

producer: producer.cpp
	$(CXX) $(CXXFLAGS) -o producer producer.cpp $(LDFLAGS)

consumer: consumer.cpp
	$(CXX) $(CXXFLAGS) -o consumer consumer.cpp $(LDFLAGS)

multi_consumer: multi_consumer.cpp
	$(CXX) $(CXXFLAGS) -o multi_consumer multi_consumer.cpp $(LDFLAGS)

clean:
	rm -f producer consumer multi_consumer

run_producer:
	./producer

run_consumer:
	./consumer

run_multi:
	./multi_consumer 1 &
	./multi_consumer 2 &
	./multi_consumer 3 &

.PHONY: all clean run_producer run_consumer run_multi