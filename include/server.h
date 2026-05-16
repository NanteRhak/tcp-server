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

<<<<<<< HEAD
/* Prototypes */
void handle_client(int connfd, int conn_number);
=======
void handle_client_iter(int connfd, int num);
void daemonize(const char *pidfile);
char *get_client_ip(struct sockaddr_in *addr, char *buf, int len);
int write_pid(const char *pidfile);
int read_pid(const char *pidfile);
>>>>>>> 1619af4 (Partie 5 - Daemon et syslog + README complet)

#endif
