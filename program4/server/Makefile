CC=g++
CXXFLAGS=-g -Wall -std=gnu++0x

ALL=chatserver.o server.o Message.o TCPConnection.o TCPServer.o

all: chatserver

chatserver: $(ALL)
	$(CC) -lpthread $(CXXFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CXXFLAGS) -o $@ $^

clean:
	rm -rf *.o chatserver  &>/dev/null
