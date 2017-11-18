/* udpserver.c
A simple UDP network server to implement Programming Assignment #2
Usage: ./udpserver <port number> <encryption key>
Author: Kat Herring
NetID: kherring
Date: 9/12/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>

#define SUCCESS 0
#define ERROR 1
#define MAX 4097

int main(int argc, char * argv[]) {
    char buf[MAX];
    char timestamp[30];
    int port; // 41021
    char *key;
    int recvlen, sendlen, n, i, s;
    socklen_t addrlen;
    struct sockaddr_in addr;
    unsigned int j;

    // process the command line arguments
    if (argc == 3) {
        port = atoi(argv[1]);
        key = argv[2];
    } else {
        fprintf(stderr, "usage: ./udpclient [port] [key]\n");
        exit(ERROR);
    }

    // build address data structure
    bzero((char *)&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // active open of socket
    if ((s = socket(PF_INET,SOCK_DGRAM,0)) < 0) {
        fprintf(stderr, "ERR: socket does not exist");
        exit(ERROR);
    }

    // bind to socket
    if (bind(s,(struct sockaddr *)&addr,sizeof(addr)) < 0) {
        fprintf(stderr, "ERR: socket could not bind");
        exit(ERROR);
    }
    addrlen = sizeof(addr);

    while (1) {
        if ((recvlen = recvfrom(s,buf,sizeof(buf),0,(struct sockaddr *)&addr,&addrlen))==-1) {
            fprintf(stderr, "ERR: did not receive message from the client\n");
            exit(ERROR);
        }
        // timestamp
        time_t timeval;
        time(&timeval);
        struct tm *tv = localtime(&timeval);
        struct timeval t;
        gettimeofday(&t,NULL);
        sprintf(timestamp," Timestamp: %02d:%02d:%02d.%d",tv->tm_hour,tv->tm_min,tv->tm_sec,t.tv_usec);
        // encrypt

        for (i=0;i<recvlen;i++)
            buf[i] = buf[i] ^ key[i%strlen(key)];
        for (j=0;j<strlen(timestamp);i++,j++) // append timestamp
            buf[i] = timestamp[j] ^ key[i%strlen(key)];
	buf[i] = '\0';
        sendlen = recvlen+strlen(timestamp);
	      // send string
        if ((n = sendto(s,buf,sendlen,0,(struct sockaddr *)&addr,addrlen))==-1) {
            fprintf(stderr, "ERR: text not sent to client: %s\n",buf);
            exit(ERROR);
        }
        // send key
        if ((n = sendto(s,key,strlen(key),0,(struct sockaddr *)&addr,addrlen))==-1) {
            fprintf(stderr, "ERR: text not sent to client: %s\n",key);
            exit(ERROR);
        }
        bzero((char *)&buf,sizeof(buf));
    }

    // close socket
    if (close(s) != 0) {
        fprintf(stderr, "ERR: socket with fd %d was not closed\n",s);
        exit(ERROR);
    }
  return SUCCESS;
}
