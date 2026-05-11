#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "server.h"

static int *connexions_actives;

static void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        (*connexions_actives)--;
}

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in srv;
    int conn_num = 0;
    int opt = 1;

    connexions_actives = mmap(NULL, sizeof(int),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *connexions_actives = 0;

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

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

    printf("Serveur fork démarré sur le port %d\n", PORT);

    while (1) {
        connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) { perror("accept"); continue; }

        pid_t pid = fork();
        if (pid < 0) { perror("fork"); close(connfd); continue; }

        if (pid == 0) {
            close(listenfd);
            (*connexions_actives)++;
            handle_client_iter(connfd, ++conn_num);
            close(connfd);
            exit(0);
        }
        close(connfd);
        printf("Connexions actives : %d\n", *connexions_actives);
    }
    return 0;
}
