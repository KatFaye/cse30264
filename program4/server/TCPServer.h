// TCP server class definition

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

#include "TCPConnection.h"

using namespace std;

class TCPServer {
	public:
		TCPServer();
		void start_server(char* port);
		TCPConnection& accept_connection();

	private:
		int conxnSock;
		int dataSock;
		struct sockaddr_in addrServer;
		struct sockaddr_in addrClient;
		socklen_t addrClient_size;

		void openSocket();
		void bindSocket();
		void listenSocket();

};

#endif
