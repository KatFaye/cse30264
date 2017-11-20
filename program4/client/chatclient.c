/* chatclient.c
driver function for client
Author: Kat Herring
Date: 11/18/2017
*/

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>

#include "client.h"


#define BUFF_SIZE 4096
#define SUCCESS 0
#define ERROR 1

using namespace std;

int main(int argc, char *argv[]) {

	if (argc != 4) {
		cerr << "Usage: ./chatclient <Server Host> <Port Number> <Username>" << endl;
		return ERROR;
	}

	char *buf = (char *)malloc(BUFF_SIZE);
	bzero(buf,BUFF_SIZE);

	Client client(argv[1], argv[2], argv[3]);
	client.start();

	return SUCCESS;
}
