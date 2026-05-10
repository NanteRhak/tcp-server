#include "../include/server.h"

/**
 * @brief Gère la communication avec un client spécifique
 * @param connfd Descripteur de socket du client
 * @param client_num Numéro séquentiel du client
 */
void handle_client(int connfd, int client_num) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE + 50];
    ssize_t n;

    while ((n = read(connfd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[n] = '\0';
        snprintf(response, sizeof(response), "[Connexion #%d] Echo : %s", client_num, buffer);
        
        if (write(connfd, response, strlen(response)) < 0) {
            perror("write");
            break;
        }
    }
    
    close(connfd);
    printf("[Fils] Client #%d déconnecté.\n", client_num);
}
