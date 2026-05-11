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
