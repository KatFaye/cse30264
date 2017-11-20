#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>

#include "TCPServer.h"
#include "TCPConnection.h"

using namespace std;

void TCPServer::openSocket() {

	if((this->conxnSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Err: socket() failed");
		exit(1);
	}

}

void TCPServer::bindSocket() {

	// allow for port reuse
	int opt = 0;
	setsockopt(this->conxnSock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(int));

	// bind socket
	if((bind(this->conxnSock, (struct sockaddr *) &this->addrServer, sizeof(this->addrServer))) < 0) {
		perror("Err: bind() failed");
		close(this->conxnSock);
		exit(1);
	}
};

void TCPServer::listenSocket() {

	if(listen(this->conxnSock, 1) == -1) {
		perror("Err: listen() failed");
		close(this->conxnSock);
		exit(1);
	}
}

TCPServer::TCPServer() {}

TCPConnection& TCPServer::accept_connection() {

	int dataSock;
	if((dataSock = accept(this->conxnSock, (struct sockaddr *) &this->addrClient, &this->addrClient_size)) == -1) {
		perror("Err: accept() failed");
		close(this->conxnSock);
		exit(1);
	}

	TCPConnection* conn = new TCPConnection(dataSock);
	return *conn;
}

void TCPServer::start_server(char *port) {

	// create server address struct
	bzero((char *) &this->addrServer, sizeof(struct sockaddr_in));
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = INADDR_ANY;
	addrServer.sin_port = htons(atoi(port));
	this->addrClient_size = sizeof(this->addrClient);

	openSocket();
	bindSocket();
	listenSocket();
}
