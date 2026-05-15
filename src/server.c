<<<<<<< HEAD
=======
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
<<<<<<< HEAD
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
>>>>>>> 60622f8 (Partie 2 - Serveur concurrent avec fork)
=======
#include <pthread.h>
>>>>>>> fcd3f5b (Partie 3 - Serveur multi-threade avec pthreads)
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
    int connection_count = 0;

<<<<<<< HEAD
<<<<<<< HEAD
    // 1. Création de la socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
=======
    connexions_actives = mmap(NULL, sizeof(int),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *connexions_actives = 0;

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

=======
>>>>>>> fcd3f5b (Partie 3 - Serveur multi-threade avec pthreads)
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) { perror("socket"); exit(1); }
>>>>>>> 60622f8 (Partie 2 - Serveur concurrent avec fork)

    // 2. Configuration SO_REUSEADDR 
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // 3. Préparation de l'adresse et Bind
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 4. Mise en écoute
    if (listen(listenfd, BACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

<<<<<<< HEAD
<<<<<<< HEAD
    printf("Serveur itératif démarré sur le port %d...\n", PORT);
=======
    printf("Serveur fork démarré sur le port %d\n", PORT);
>>>>>>> 60622f8 (Partie 2 - Serveur concurrent avec fork)
=======
    printf("Serveur threads démarré sur le port %d\n", PORT);
>>>>>>> fcd3f5b (Partie 3 - Serveur multi-threade avec pthreads)

    // 5. Boucle principale 
    while (1) {
        connfd = accept(listenfd, NULL, NULL);
<<<<<<< HEAD
        if (connfd < 0) {
            perror("accept");
            continue; // On continue malgré l'erreur
        }

        connection_count++;
        handle_client(connfd, connection_count);
        
        close(connfd); // IMPORTANT en itératif : fermer avant le prochain accept
=======
        if (connfd < 0) { perror("accept"); continue; }

        pthread_mutex_lock(&mutex);
        if (connexions_actives >= MAX_THREADS) {
            pthread_mutex_unlock(&mutex);
            char *msg = "Serveur saturé, réessayez plus tard.\n";
            write(connfd, msg, strlen(msg));
            close(connfd);
            continue;
        }
<<<<<<< HEAD
        close(connfd);
        printf("Connexions actives : %d\n", *connexions_actives);
>>>>>>> 60622f8 (Partie 2 - Serveur concurrent avec fork)
=======
        pthread_mutex_unlock(&mutex);

        int *fd_copy = malloc(sizeof(int));
        *fd_copy = connfd;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client_thread, fd_copy);
        pthread_detach(tid);
>>>>>>> fcd3f5b (Partie 3 - Serveur multi-threade avec pthreads)
    }

    close(listenfd);
    return 0;
}
