.PHONY: all
.PHONY: clean

PORT=8585
SERVER_ADDRESS='"127.0.0.1"'
SERVER_BIND_ADDRESS='"127.0.0.1"'
DEFINES=-DPORT=${PORT} -DSERVER_ADDRESS=${SERVER_ADDRESS} -DSERVER_BIND_ADDRESS=${SERVER_BIND_ADDRESS}
HEADERS=network_utils.h document.h
SOURCES=server.cpp operation.cpp document.cpp network_utils.cpp

all: server

run: all
	./server

server: ${SOURCES} ${HEADERS}
	g++ -pthread -std=c++11 ${DEFINES} ${SOURCES} -o server

clean:
	rm -f server

