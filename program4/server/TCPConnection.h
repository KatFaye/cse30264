/* TCPConnection.h
defintion of TCPConnection class
Author: Kat Herring
Date: 11/18/2017
*/

#ifndef _TCP_CONNECTION
#define _TCP_CONNECTION

#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <queue>
#include <vector>
#include <fcntl.h>
#include <errno.h>

#include "Message.h"

using namespace std;

class TCPConnection {
	public:
		TCPConnection();
		TCPConnection(int dataSock);
		void run(char *host, char *port);
		void sendMessage(Message message);
		bool checkMsgs();
		Message* newestMsg();
		void popMsg();
		void closeSock();

	private:
		char *buf;
		int buf_size;
		int dataSock;
		char *host;
		struct hostent *hp;
		struct sockaddr_in addrClient;
		bool is_connected;
		queue<Message> message_queue;
		const static char message_delimiter = '|';

		string convertStr(char* buf);
		string rstrip(string str);
		void openSocket(int &sockfd);
		void connSockClient();
		void sendMessage(string s);
		string msgEncode(string message_string);
		string msgDecode(string encoded_string);
		void msgsToQueue();
		string recvSocket();
		void msgSplit(string incoming_messages, vector<string>& messages);

};

#endif
