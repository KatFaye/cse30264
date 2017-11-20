/*
server.h
server class definition
Author: Kat Herring
Date: 11/18/2017
*/

#ifndef _SERVER
#define _SERVER

#include <unordered_map>
#include <pthread.h>
#include <vector>

#include "TCPServer.h"

using namespace std;

class Server {
public:
    Server(char *);
    void accept_connections();
private:
  string rstrip(string);
  void split(string, string &, string &);
  static void *handleClient(void *);
  Message idle(TCPConnection);
  Message ackWait(TCPConnection);
  string onlineNow();
  void broadcast(Message, string);
  bool msgPrivate(Message, string, string);

  int port;
  TCPServer tcp_server;
  std::vector<pthread_t> threads;
  unordered_map<string, TCPConnection> connections;
  unordered_map<string, string> passwords;
};

#endif // _SERVER
