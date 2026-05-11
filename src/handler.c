#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "server.h"

/*
 * handle_client_iter - Gère un client en mode itératif
 * @connfd : descripteur de la connexion client
 * @num    : numéro séquentiel de la connexion
 * Retour  : void
 */
void handle_client_iter(int connfd, int num) {
    char buf[BUF_SIZE];
    char response[BUF_SIZE + 64];
    ssize_t n;

    n = read(connfd, buf, sizeof(buf) - 1);
    if (n < 0) { perror("read"); return; }
    if (n == 0) return;

    buf[n] = '\0';
    snprintf(response, sizeof(response), "[Connexion #%d] Echo : %s", num, buf);

    if (write(connfd, response, strlen(response)) < 0)
        perror("write");
}
