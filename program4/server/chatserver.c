/* chatserver.c
driver function for server
Author: Kat Herring
Date: 11/18/2017
*/

#include <cstdlib>
#include <iostream>

#include "server.h"
#define SUCCESS 0
#define ERROR 1

using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: ./chatserver <Port Number>" << endl;
    return ERROR;
  }

  Server server(argv[1]);
  server.accept_connections();

return SUCCESS;
}
