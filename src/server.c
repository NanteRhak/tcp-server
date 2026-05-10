#include "server.h"

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in srv;
    int opt = 1;
    int connection_count = 0;

    // 1. Création de la socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

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

    printf("Serveur itératif démarré sur le port %d...\n", PORT);

    // 5. Boucle principale 
    while (1) {
        connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) {
            perror("accept");
            continue; // On continue malgré l'erreur
        }

        connection_count++;
        handle_client(connfd, connection_count);
        
        close(connfd); // IMPORTANT en itératif : fermer avant le prochain accept
    }

    close(listenfd);
    return 0;
}
