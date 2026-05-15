#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "server.h"

#define MAX_THREADS 16

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int connexions_actives = 0;

void afficher_statut(void) {
    pthread_mutex_lock(&mutex);
    printf("Connexions actives : %d / %d\n", connexions_actives, MAX_THREADS);
    pthread_mutex_unlock(&mutex);
}

void *handle_client_thread(void *arg) {
    int connfd = *(int *)arg;
    free(arg);

    pthread_mutex_lock(&mutex);
    connexions_actives++;
    pthread_mutex_unlock(&mutex);

    afficher_statut();

    char buf[BUF_SIZE];
    char response[BUF_SIZE + 64];
    ssize_t n = read(connfd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        static int conn_num = 0;
        conn_num++;
        snprintf(response, sizeof(response), "[Connexion #%d] Echo : %s", conn_num, buf);
        write(connfd, response, strlen(response));
    }
    close(connfd);

    pthread_mutex_lock(&mutex);
    connexions_actives--;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in srv;
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

    printf("Serveur threads démarré sur le port %d\n", PORT);

    while (1) {
        connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) { perror("accept"); continue; }

        pthread_mutex_lock(&mutex);
        if (connexions_actives >= MAX_THREADS) {
            pthread_mutex_unlock(&mutex);
            char *msg = "Serveur saturé, réessayez plus tard.\n";
            write(connfd, msg, strlen(msg));
            close(connfd);
            continue;
        }
        pthread_mutex_unlock(&mutex);

        int *fd_copy = malloc(sizeof(int));
        *fd_copy = connfd;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client_thread, fd_copy);
        pthread_detach(tid);
    }
    return 0;
}
