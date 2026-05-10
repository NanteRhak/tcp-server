#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 9999
#define BACKLOG 10
#define BUFFER_SIZE 1024

/* Prototypes */
void handle_client(int connfd, int conn_number);

#endif
