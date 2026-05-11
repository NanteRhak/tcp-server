# TCP Server UNIX — Tino, Vahatra, Elvys
# Licence 3 Informatique — Systèmes & Réseaux

## Partie 1 — Serveur TCP itératif

### Description
Serveur TCP minimaliste en mode itératif. Il traite un client à la fois.
La socket est créée avec socket(), configurée avec SO_REUSEADDR, bind() sur
le port 9999 et listen() avec un backlog de 10.

### Compilation et lancement
    make
    ./tcp_server

### Test effectué
    tino@ubuntu:~/tcp-server$ ./tcp_server
    Serveur itératif démarré sur le port 9999

    tino@ubuntu:~$ echo "Bonjour" | nc 127.0.0.1 9999
    [Connexion #1] Echo : Bonjour

### Comportement itératif observé
Deux clients simultanés ne sont pas servis en parallèle. Le second client
attend que le premier ait terminé. Le serveur a une seule boucle
accept/read/write. Tant qu'il traite le client 1, il ne revient pas à
accept(). Le client 2 reste dans la file d'attente du noyau (backlog).

## Partie 2 — Serveur concurrent avec fork()

### Description
Serveur concurrent basé sur fork(). Chaque connexion cliente est prise en
charge par un processus fils indépendant. Un gestionnaire SIGCHLD avec
waitpid(-1, NULL, WNOHANG) évite les processus zombies. Le compteur de
connexions actives est partagé via mmap MAP_SHARED | MAP_ANONYMOUS.

### Test avec 8 clients simultanés
    tino@ubuntu:~$ for i in $(seq 1 8); do
        (echo "Client $i : bonjour" | nc -q 1 127.0.0.1 9999) &
    done
    wait
    echo 'Tous les clients ont terminé'
    [Connexion #1] Echo : Client 4 : bonjour
    [Connexion #1] Echo : Client 5 : bonjour
    [Connexion #1] Echo : Client 8 : bonjour
    [Connexion #1] Echo : Client 1 : bonjour
    [Connexion #1] Echo : Client 2 : bonjour
    [Connexion #1] Echo : Client 3 : bonjour
    [Connexion #1] Echo : Client 7 : bonjour
    [Connexion #1] Echo : Client 6 : bonjour
    Tous les clients ont terminé

### Résultat ps aux
    tino  19863  0.0  0.0   2684  1668 pts/3  S+  17:36  0:00 ./tcp_server

### Analyse
Le père (PID 19863) reste en écoute en permanence. Les fils ont des PIDs
éphémères non capturables car ils se terminent avant la commande ps. Les
clients arrivent dans le désordre ce qui prouve qu'ils sont traités en
parallèle. La solution IPC choisie est mmap car elle est partagée entre
le père et tous les fils avant fork(), plus rapide qu'un fichier temporaire.
