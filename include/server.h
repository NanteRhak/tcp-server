#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#define PORT     9999
#define BACKLOG  10
#define BUF_SIZE 1024

void handle_client_iter(int connfd, int num);

#endif
