// udpclient.c
// A simple UDP network client
// Programming Assignment #1
// Author: Kat Herring
// Date: 9/12/2017

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SUCCESS = 0
#define ERROR = 1
#define MAX = 4096

int main(int argc, char **argv) {
  char *txt = ".txt";
  char *initmsg = "Testing connection.";
  char buffer[MAX];
  int initmsglen = strlen(initmsg);
  int socketfd, sent, got, length;

  struct addrinfo *serverInfo, *res, hints;
  socklen_t addrlen;



  if (argc != 4) {
    return ERROR;
  }

  char hostName = argv[1];
  char portNum = argv[2];
  char inputText = argv[3];

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  // get server info
  if (getaddrinfo(hostName, portNum, &hints, &serverInfo)) {
    return ERROR;
  }

  // init UDP socket
  for (res = serverInfo; res != NULL; res = res->ai_next) {
    socketfd = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (socketfd < 0) {
      continue;
    }
    break; // success
  }

  if (!res) {
    return ERROR;
  }

  // send initial message
  addrlen = sizeof(serverInfo->ai_addr);
  length = addrlen;

  sent = sendto(socketfd, initmsg, initmsglen, 0, serverInfo->ai_addr, addrlen);

  // first response from server
  got = recvfrom(socketfd, buffer, MAX-1, 0, serverInfo->ai_addr, length);
  if (got < 0) {
    return ERROR;
  }
  buffer[got] = '\0';
  printf("Resultant key: %s\n", buffer);
  // if (strstr(inputText, txt)) { // is text file
  //
  // } else { // string message
  //
  // }

  // clean up
  close(socketfd);
  freeaddrinfo(serverInfo);
  return SUCCESS;
}
