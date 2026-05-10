#include "server.h"

/**
 * Gère la communication avec un client (Modèle itératif)
 * @param connfd Descripteur de la socket client
 * @param conn_number Numéro séquentiel de la connexion
 */
void handle_client(int connfd, int conn_number) {
    char buffer[BUFFER_SIZE];
    ssize_t n;

    // Lecture des données envoyées par le client
    n = read(connfd, buffer, sizeof(buffer) - 1);
    if (n < 0) {
        perror("Erreur read");
        return;
    }
    
    buffer[n] = '\0'; // Sécurité pour l'affichage

    // Préparation de la réponse avec le numéro de connexion
    char response[BUFFER_SIZE + 50];
    snprintf(response, sizeof(response), "[Connexion #%d] Echo : %s", conn_number, buffer);

    // Renvoi au client
    if (write(connfd, response, strlen(response)) < 0) {
        perror("Erreur write");
    }
}
