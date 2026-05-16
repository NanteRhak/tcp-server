<<<<<<< HEAD
=======
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
<<<<<<< HEAD
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
=======
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <syslog.h>
#include <arpa/inet.h>
>>>>>>> 1619af4 (Partie 5 - Daemon et syslog + README complet)
#include "server.h"

static int *connexions_actives;

static void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        (*connexions_actives)--;
}

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in srv, cli;
    socklen_t clilen = sizeof(cli);
    int conn_num = 0;
    int opt = 1;
<<<<<<< HEAD
<<<<<<< HEAD
    int connection_count = 0;
=======
    int clients[FD_SETSIZE];
    int i, maxi = -1, nready, maxfd;
    fd_set rset, allset;

    for (i = 0; i < FD_SETSIZE; i++) clients[i] = -1;
>>>>>>> fa94fc2 (Partie 4 - Multiplexage IO avec select)
=======

    openlog("myserverd", LOG_PID | LOG_CONS, LOG_DAEMON);

    daemonize("/tmp/myserverd.pid");

    connexions_actives = mmap(NULL, sizeof(int),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *connexions_actives = 0;

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);
>>>>>>> 1619af4 (Partie 5 - Daemon et syslog + README complet)

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
<<<<<<< HEAD
    if (listenfd < 0) { perror("socket"); exit(1); }
>>>>>>> 60622f8 (Partie 2 - Serveur concurrent avec fork)
=======
    if (listenfd < 0) { syslog(LOG_ERR, "socket() échoué : %m"); exit(1); }
>>>>>>> 1619af4 (Partie 5 - Daemon et syslog + README complet)

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

<<<<<<< HEAD
    if (bind(listenfd, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
=======
    if (bind(listenfd, (struct sockaddr*)&srv, sizeof(srv)) < 0) {
        syslog(LOG_ERR, "bind() échoué : %m"); exit(1);
>>>>>>> 1619af4 (Partie 5 - Daemon et syslog + README complet)
    }

    // 4. Mise en écoute
    if (listen(listenfd, BACKLOG) < 0) {
<<<<<<< HEAD
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
=======
        syslog(LOG_ERR, "listen() échoué : %m"); exit(1);
    }

    syslog(LOG_INFO, "Daemon démarré sur le port %d", PORT);
>>>>>>> 1619af4 (Partie 5 - Daemon et syslog + README complet)

    // 5. Boucle principale 
    while (1) {
<<<<<<< HEAD
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
=======
        connfd = accept(listenfd, (struct sockaddr*)&cli, &clilen);
        if (connfd < 0) { syslog(LOG_WARNING, "accept() échoué : %m"); continue; }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cli.sin_addr, ip, sizeof(ip));
        syslog(LOG_INFO, "Connexion acceptée de %s:%d", ip, ntohs(cli.sin_port));
>>>>>>> 1619af4 (Partie 5 - Daemon et syslog + README complet)

        pid_t pid = fork();
        if (pid < 0) { syslog(LOG_ERR, "fork() échoué : %m"); close(connfd); continue; }

<<<<<<< HEAD
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
=======
        if (pid == 0) {
            close(listenfd);
            (*connexions_actives)++;
            handle_client_iter(connfd, ++conn_num);
            syslog(LOG_INFO, "Client traité (fd=%d)", connfd);
            close(connfd);
            exit(0);
        }
        close(connfd);
    }

    closelog();
>>>>>>> 1619af4 (Partie 5 - Daemon et syslog + README complet)
    return 0;
}
