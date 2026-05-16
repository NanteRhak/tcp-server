<<<<<<< HEAD
=======
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>
<<<<<<< HEAD
<<<<<<< HEAD
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
>>>>>>> 60622f8 (Partie 2 - Serveur concurrent avec fork)
=======
#include <pthread.h>
>>>>>>> fcd3f5b (Partie 3 - Serveur multi-threade avec pthreads)
=======
>>>>>>> fa94fc2 (Partie 4 - Multiplexage IO avec select)
#include "server.h"

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in srv;
    int opt = 1;
<<<<<<< HEAD
    int connection_count = 0;
=======
    int clients[FD_SETSIZE];
    int i, maxi = -1, nready, maxfd;
    fd_set rset, allset;

    for (i = 0; i < FD_SETSIZE; i++) clients[i] = -1;
>>>>>>> fa94fc2 (Partie 4 - Multiplexage IO avec select)

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
<<<<<<< HEAD
    printf("Serveur itératif démarré sur le port %d...\n", PORT);
=======
    printf("Serveur fork démarré sur le port %d\n", PORT);
>>>>>>> 60622f8 (Partie 2 - Serveur concurrent avec fork)
=======
    printf("Serveur threads démarré sur le port %d\n", PORT);
>>>>>>> fcd3f5b (Partie 3 - Serveur multi-threade avec pthreads)
=======
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    maxfd = listenfd;

    printf("Serveur select démarré sur le port %d\n", PORT);
>>>>>>> fa94fc2 (Partie 4 - Multiplexage IO avec select)

    // 5. Boucle principale 
    while (1) {
<<<<<<< HEAD
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
=======
        rset = allset;
        struct timeval tv = {5, 0};
        nready = select(maxfd + 1, &rset, NULL, NULL, &tv);
        if (nready < 0) { perror("select"); continue; }
        if (nready == 0) { printf("Timeout, en attente...\n"); continue; }
>>>>>>> fa94fc2 (Partie 4 - Multiplexage IO avec select)

        if (FD_ISSET(listenfd, &rset)) {
            connfd = accept(listenfd, NULL, NULL);
            for (i = 0; i < FD_SETSIZE; i++) {
                if (clients[i] == -1) {
                    clients[i] = connfd;
                    if (i > maxi) maxi = i;
                    break;
                }
            }
            FD_SET(connfd, &allset);
            if (connfd > maxfd) maxfd = connfd;
            if (--nready <= 0) continue;
        }
<<<<<<< HEAD
<<<<<<< HEAD
        close(connfd);
        printf("Connexions actives : %d\n", *connexions_actives);
>>>>>>> 60622f8 (Partie 2 - Serveur concurrent avec fork)
=======
        pthread_mutex_unlock(&mutex);
=======
>>>>>>> fa94fc2 (Partie 4 - Multiplexage IO avec select)

        for (i = 0; i <= maxi; i++) {
            if (clients[i] == -1) continue;
            if (FD_ISSET(clients[i], &rset)) {
                char buf[BUF_SIZE];
                char response[BUF_SIZE + 64];
                ssize_t n = read(clients[i], buf, sizeof(buf) - 1);
                if (n <= 0) {
                    close(clients[i]);
                    FD_CLR(clients[i], &allset);
                    clients[i] = -1;
                } else {
                    buf[n] = '\0';
                    snprintf(response, sizeof(response), "Echo : %s", buf);
                    write(clients[i], response, strlen(response));
                }

<<<<<<< HEAD
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client_thread, fd_copy);
        pthread_detach(tid);
>>>>>>> fcd3f5b (Partie 3 - Serveur multi-threade avec pthreads)
=======
                int count = 0;
                for (int j = 0; j < FD_SETSIZE; j++)
                    if (clients[j] != -1) count++;
                printf("Descripteurs surveillés : %d\n", count);

                if (--nready <= 0) break;
            }
        }
>>>>>>> fa94fc2 (Partie 4 - Multiplexage IO avec select)
    }

    close(listenfd);
    return 0;
}
