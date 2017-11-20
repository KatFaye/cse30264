CC=g++
CXXFLAGS=-g -Wall -std=gnu++0x

ALL=chatclient.o client.o Message.o TCPConnection.o

all: chatclient

chatclient: $(ALL)
	$(CC) -lpthread $(CXXFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CXXFLAGS) -o $@ $^

clean:
	rm -rf *.o chatclient  &>/dev/null
