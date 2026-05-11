#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.h"

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in srv;
    int conn_num = 0;
    int opt = 1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) { perror("socket"); exit(1); }

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&srv, 0, sizeof(srv));
    srv.sin_family      = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port        = htons(PORT);

    if (bind(listenfd, (struct sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("bind"); exit(1);
    }
    if (listen(listenfd, BACKLOG) < 0) {
        perror("listen"); exit(1);
    }

    printf("Serveur itératif démarré sur le port %d\n", PORT);

    while (1) {
        connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) { perror("accept"); continue; }
        handle_client_iter(connfd, ++conn_num);
        close(connfd);
    }
    return 0;
}
