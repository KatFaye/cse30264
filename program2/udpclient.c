/* udpclient.c
A simple UDP network client to implement Programming Assignment #1
Usage: ./udpclient <server host name> <port number> <file/string>
Author: Kat Herring
NetID: kherring
Date: 9/12/2017
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdint.h>

#define SUCCESS 0
#define ERROR 1
#define MAX 4097

void decrypt(char *msg, char *key, char *decryption, int size) {
  int i;

  for (i=0; i<size; i++) {
    decryption[i] = msg[i] ^ key[i % strlen(key)];
  }
}

int main(int argc, char **argv) {
  char *txt = ".txt";
  char *initmsg = "Testing connection.";
  char *hostName, *portNum, *inputText, *tok;
  char buffer[MAX], key[MAX], decrypted[MAX], fileBuff[MAX];
  char tstamp[20] = { '\0' };
  int initmsglen = strlen(initmsg);
  int socketfd, sent, got, i, usec;
  int j = 0;
  struct timeval end;

  struct addrinfo *serverInfo, *res, hints;
  socklen_t addrlen;



  if (argc != 4) {
    return ERROR;
  }

  hostName = argv[1];
  portNum = argv[2];
  inputText = argv[3];

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
  sent = sendto(socketfd, initmsg, initmsglen, 0, serverInfo->ai_addr, serverInfo->ai_addrlen);
  if (sent < 0) {
    return ERROR;
  }
  // get encrpytion key from server
  got = recvfrom(socketfd, buffer, MAX-1, 0, serverInfo->ai_addr, &addrlen);
  if (got < 0) {
    return ERROR;
  }
  buffer[got] = '\0';
  strcpy(key, buffer);


  if (strstr(inputText, txt)) { // is text file
    FILE *fp;

    fp = fopen(inputText, "r");
    if (!fp) {
      return ERROR;
    }
    fgets(fileBuff, MAX, (FILE *)fp);
    sent = sendto(socketfd, fileBuff, strlen(fileBuff), 0, serverInfo->ai_addr, serverInfo->ai_addrlen);
    if (sent < 0) {
      return ERROR;
    }

  } else { // string message
    sent = sendto(socketfd, inputText, strlen(inputText), 0, serverInfo->ai_addr, serverInfo->ai_addrlen);
    if (sent < 0) {
      return ERROR;
    }
  }

  // get encrypted message from server

  got = recvfrom(socketfd, buffer, MAX-1, 0, serverInfo->ai_addr, &addrlen);
  if (got < 0 ) {
    return ERROR;
  }
  buffer[got] = '\0';
  decrypt(buffer, key, decrypted, got);
  decrypted[got] = '\0';
  printf("Decrypted Message: %s \n", decrypted);

  tok = strchr(decrypted, ':');
  tok += 2;
  //printf("%s\n", tok);

  while(*tok != '\0') {
    //printf("%c\n", *tok);
    tstamp[j] = *tok;
    tok += 1;
    j += 1;
  }
  //printf("%s\n", tstamp);

  tok = strchr(tstamp, '.');
  tok += 1;

  //printf("%s\n", tok);
  usec = atoi(tok);
  gettimeofday(&end, NULL);


  printf("Round Trip Time (RTT): %i microseconds\n", (abs(usec-end.tv_usec)));
  // clean up
  close(socketfd);
  freeaddrinfo(serverInfo);
  return SUCCESS;
}
