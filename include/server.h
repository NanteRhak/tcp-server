#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define PORT 9999
#define BACKLOG 10
#define BUFFER_SIZE 1024

// Gestionnaire de client (sera dans handler.c)
void handle_client(int connfd, int client_num);

#endif
