# Compiler and flags
CFLAGS := -c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS := -lboost_system -lpthread

# Target binaries
all: StompWCIClient

StompWCIClient: bin/ConnectionHandler.o bin/StompProtocol.o bin/event.o bin/StompMessageParser.o bin/Auxiliary.o bin/StompClient.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/StompProtocol.o bin/event.o bin/StompMessageParser.o bin/Auxiliary.o bin/StompClient.o $(LDFLAGS)

# Rules for object files
bin/ConnectionHandler.o: src/ConnectionHandler.cpp include/ConnectionHandler.h
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/StompProtocol.o: src/StompProtocol.cpp include/StompProtocol.h
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/event.o: src/event.cpp include/event.h
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompMessageParser.o: src/StompMessageParser.cpp include/StompMessageParser.h
	g++ $(CFLAGS) -o bin/StompMessageParser.o src/StompMessageParser.cpp

bin/Auxiliary.o: src/Auxiliary.cpp include/Auxiliary.h
	g++ $(CFLAGS) -o bin/Auxiliary.o src/Auxiliary.cpp

bin/StompClient.o: src/StompClient.cpp include/StompProtocol.h
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

# Clean rule
.PHONY: clean
clean:
	rm -f bin/*
