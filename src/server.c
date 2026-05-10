#include "../include/server.h"

// Compteur partagé en mémoire vive
static int *active_connections;

/**
 * @brief Nettoie les processus fils pour éviter les zombies
 */
void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in srv;
    int client_count = 0;

    // 1. Création de la mémoire partagée pour le compteur
    active_connections = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, 
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *active_connections = 0;

    // 2. Configuration Socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    bind(listenfd, (struct sockaddr *)&srv, sizeof(srv));
    listen(listenfd, BACKLOG);

    // 3. Gestion du signal SIGCHLD
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    printf("Serveur concurrent (fork) démarré sur le port %d...\n", PORT);

    while (1) {
        connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) continue;

        client_count++;
        (*active_connections)++; // Incrément partagé

        pid_t pid = fork();
        if (pid == 0) { 
            // --- PROCESSUS FILS ---
            close(listenfd); // Le fils n'écoute pas les nouveaux clients
            handle_client(connfd, client_count);
            
            exit(0); 
        } else if (pid > 0) {
            // --- PROCESSUS PÈRE ---
            close(connfd); // Le père ne parle pas au client
            printf("[Père] Client #%d connecté. Total actifs: %d\n", 
                    client_count, *active_connections);
        }
    }
    return 0;
}
