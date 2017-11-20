/* client.h
definition of Client class
Author: Kat Herring
Date: 11/18/2017
*/

#ifndef _CLIENT
#define CLIENT

#include <pthread.h>
#include <stdio.h>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "TCPConnection.h"
using namespace std;

class Client {

	public:
		Client(char *, char *, char *);
		void start();
		void usage();
		void openSocket();
		void connectSocket();
		void sendMessage(string s);
		void closeSock();

	private:
		void msgBroadcast();
		void msgPrivate();
		void logout();
		void login(char *);
		static void *msgHandler(void *);
		Message ackWait();

		string strip(string s);
		TCPConnection conxn;
		pthread_t t_msgHandler;

};

#endif // _CLIENT
