# TCP Server UNIX
**Auteurs :**
- FIONONANA RANDRY Tino
- RAFANOMEZANTSOA Holinirina Vahatriniaina
- RAKOTOVAO Nantenaina Elvys

**Licence 3 Télécommunications**

## Partie 1 — Serveur TCP itératif

### Description
Serveur TCP minimaliste en mode itératif. Il traite un client à la fois.
La socket est créée avec socket(), configurée avec SO_REUSEADDR, bind() sur
le port 9999 et listen() avec un backlog de 10.

### Compilation et lancement
    make
    ./tcp_server

### Test 1 — Echo basique
    $ echo "Bonjour" | nc 127.0.0.1 9999
    [Connexion #1] Echo : Bonjour

### Test 2 — Blocage itératif
    Terminal 2 : nc 127.0.0.1 9999  (connecté mais silencieux)
    Terminal 3 : echo "Hello" | nc 127.0.0.1 9999  (bloqué, pas de réponse)
    Quand Terminal 2 fait Ctrl+C :
    Terminal 3 reçoit immédiatement : [Connexion #2] Echo : Hello

### Résultat
Le terminal 3 est bloqué tant que le terminal 2 occupe le serveur.
Le serveur est bloqué sur read() du client 1 et ne peut pas accepter
de nouvelle connexion. Le client 2 reste dans la file d'attente du
noyau (backlog).

## Partie 2 — Serveur concurrent avec fork()

### Description
Serveur concurrent basé sur fork(). Chaque connexion cliente est prise en
charge par un processus fils indépendant. Un gestionnaire SIGCHLD avec
waitpid(-1, NULL, WNOHANG) évite les processus zombies. Le compteur de
connexions actives est partagé via mmap MAP_SHARED | MAP_ANONYMOUS.

### Test fork + pstree
    $ for i in $(seq 1 8); do
        (echo "Client $i : bonjour" | nc -q 1 127.0.0.1 9999) &
      done
      wait
      pstree -p $(pgrep tcp_server)
    [Connexion #1] Echo : Client 2 : bonjour
    [Connexion #1] Echo : Client 4 : bonjour
    [Connexion #1] Echo : Client 1 : bonjour
    [Connexion #1] Echo : Client 7 : bonjour
    [Connexion #1] Echo : Client 3 : bonjour
    [Connexion #1] Echo : Client 6 : bonjour
    [Connexion #1] Echo : Client 5 : bonjour
    [Connexion #1] Echo : Client 8 : bonjour
    tcp_server(9094)

### Test IPC mémoire
    $ ps -C tcp_server -o rss=
    1660 KB

### Test anti-zombies
    $ ps aux | grep Z
    tino  5671  Z  [sd_espeak-ng-mb] <defunct>  (processus système, pas lié au serveur)
    Aucun zombie lié à tcp_server — SIGCHLD fonctionne correctement.

### Résultat
Les 8 clients sont servis en parallèle dans le désordre ce qui prouve
le parallélisme. Le père (PID 9094) reste en écoute. Les fils se terminent
rapidement. Aucun zombie détecté. Mémoire : 1660 KB.

## Partie 3 — Serveur multi-threadé avec pthreads

### Description
Serveur concurrent basé sur pthreads. Chaque connexion est gérée par un
thread indépendant. Un mutex protège le compteur connexions_actives.
Pool de MAX_THREADS=16. pthread_detach() libère automatiquement les ressources.

### Test Threads — 8 clients + VmRSS
    VmRSS threads : 1720 kB
    VmRSS fork    : 1660 kB
    Les threads partagent la mémoire, les processus la dupliquent.

### Test Pool — 17 clients
    17 clients servis car connexions trop courtes pour saturer le pool.
    Le message "Serveur saturé" apparaît uniquement si 16 threads
    sont actifs simultanément.

### Test Race Condition — 16 clients
    Compteur stable après 16 connexions simultanées.
    VmRSS : 1728 kB. Le mutex protège bien le compteur global.

## Partie 4 — Multiplexage I/O avec select()

### Description
Serveur mono-thread qui surveille plusieurs clients simultanément avec select().
Un tableau clients[] de FD_SETSIZE descripteurs initialisé à -1. Timeout de
5 secondes. Aucun thread ni processus fils créé.

### Test select() — client silencieux
    Terminal 2 : nc 127.0.0.1 9999  (silencieux, ne tape rien)
    Terminal 3 : nc 127.0.0.1 9999
                 Bonjour
                 Echo : Bonjour

### Résultat
Le client silencieux ne bloque pas le client actif. select() surveille
tous les descripteurs simultanément. Descripteurs surveillés : 2.
Contrairement à la partie 1, un client inactif ne bloque pas les autres.

### Réponses select vs poll
select() est limité à FD_SETSIZE=1024 descripteurs. poll() n'a pas cette
limite. Pour 500 connexions poll() est préférable. Pour 10000+ connexions
epoll (Linux) est recommandé avec une complexité O(1).
