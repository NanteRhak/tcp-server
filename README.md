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

## Partie 3 — Serveur multi-threadé avec pthreads

### Description
Serveur concurrent basé sur pthreads. Chaque connexion cliente est prise en
charge par un thread indépendant. Un mutex protège le compteur connexions_actives.
Un pool de MAX_THREADS=16 refuse les connexions si saturé. pthread_detach()
libère automatiquement les ressources du thread à sa fin.

### Pourquoi malloc pour passer connfd au thread
Il est interdit de passer &connfd directement car connfd est une variable locale
de la boucle principale. À la prochaine itération connfd est réécrit avant que
le thread ait eu le temps de le lire, créant une race condition. La solution
est d'allouer une copie avec malloc() pour chaque thread.

### Test avec 8 clients simultanés
    tino@ubuntu:~$ for i in $(seq 1 8); do
        (echo "Client $i : bonjour" | nc -q 1 127.0.0.1 9999) &
    done
    wait
    echo 'Tous les clients ont terminé'
    [Connexion #1] Echo : Client 3 : bonjour
    [Connexion #4] Echo : Client 6 : bonjour
    [Connexion #3] Echo : Client 1 : bonjour
    [Connexion #2] Echo : Client 2 : bonjour
    [Connexion #5] Echo : Client 7 : bonjour
    [Connexion #6] Echo : Client 8 : bonjour
    [Connexion #7] Echo : Client 4 : bonjour
    [Connexion #8] Echo : Client 5 : bonjour
    Tous les clients ont terminé

### Comparaison mémoire fork vs threads
    Version fork()   : VmRSS = 1476 kB
    Version threads  : VmRSS = 1724 kB

### Analyse
Les numéros de connexion sont différents (#1, #2, #3...) grâce au mutex qui
protège le compteur. La version fork consomme moins de mémoire au repos mais
sous charge chaque fils duplique toute la mémoire du père. Les threads partagent
la mémoire donc sont plus efficaces sous forte charge.

## Partie 4 — Multiplexage I/O avec select()

### Description
Serveur mono-thread qui surveille plusieurs clients simultanément avec select().
Un tableau clients[] de FD_SETSIZE descripteurs est initialisé à -1. select()
surveille tous les descripteurs avec un timeout de 5 secondes. Aucun thread
ni processus fils n'est créé.

### Test effectué
    tino@ubuntu:~/tcp-server$ ./tcp_server
    Serveur select démarré sur le port 9999
    Timeout, en attente...
    Timeout, en attente...
    Descripteurs surveillés : 1

    tino@ubuntu:~$ for i in $(seq 1 8); do
        (echo "Client $i : bonjour" | nc -q 1 127.0.0.1 9999) &
    done
    Echo : Client 3 : bonjour
    Echo : Client 8 : bonjour
    Echo : Client 4 : bonjour
    Echo : Client 5 : bonjour
    Echo : Client 7 : bonjour
    Echo : Client 6 : bonjour
    Echo : Client 1 : bonjour
    Echo : Client 2 : bonjour
    Tous les clients ont terminé

### Réponses aux questions select vs poll
select() est limité à FD_SETSIZE = 1024 descripteurs maximum alors que poll()
utilise un tableau dynamique sans limite fixe. FD_SETSIZE=1024 est un problème
en production car un serveur peut recevoir des milliers de connexions simultanées.
Pour 500 connexions poll() est préférable car son API est plus claire et il n'est
pas nécessaire de reconstruire le fd_set à chaque appel. Pour 10 000+ connexions
la syscall recommandée est epoll (Linux) avec une complexité O(1) au lieu de
O(n) pour select/poll, utilisé par nginx, Redis et Node.js.

## Partie 5 — Daemon et syslog

### Description
Le serveur est transformé en daemon UNIX complet. Il tourne en arrière-plan
même si le terminal est fermé. Toute l'activité est tracée via syslog dans
/var/log/myserverd.log.

### Séquence de daemonisation
1. Premier fork : le père quitte, le fils continue
2. setsid() : crée une nouvelle session sans terminal de contrôle
3. Second fork : empêche toute réacquisition de terminal
4. chdir("/") : évite de bloquer un système de fichiers monté
5. umask(0) : contrôle total sur les permissions
6. Redirection stdin/stdout/stderr vers /dev/null

### Test effectué
    tino@ubuntu:~/tcp-server$ ./tcp_server
    tino@ubuntu:~/tcp-server$  (le terminal rend la main immédiatement)

    tino@ubuntu:~$ echo "Bonjour daemon" | nc 127.0.0.1 9999
    [Connexion #1] Echo : Bonjour daemon

### Logs observés dans /var/log/myserverd.log
    2026-05-11T11:02:01 ubuntu myserverd[9132]: Daemon démarré sur le port 9999
    2026-05-11T11:03:08 ubuntu myserverd[9132]: Connexion acceptée de 127.0.0.1:33816
    2026-05-11T11:03:08 ubuntu myserverd[9166]: Client traité (fd=5)

### Configuration rsyslog
Ligne ajoutée dans /etc/rsyslog.conf :
    daemon.*    /var/log/myserverd.log

### Niveaux syslog utilisés
    LOG_INFO    : connexions normales
    LOG_WARNING : erreurs récupérables (accept échoué)
    LOG_ERR     : erreurs fatales (socket, bind, fork échoués)

## Conclusion

Pour un service en production nous choisirions epoll avec un pool de threads
fixe comme le fait nginx. Le modèle fork pur est trop coûteux en mémoire sous
forte charge, le modèle select est limité à 1024 connexions et le modèle
threads pur sans pool peut saturer le système. La combinaison epoll et thread
pool est le standard industriel actuel.

## Auteurs
Tino, Vahatra, Elvys
Licence 3 Télécommunications — Systèmes & Réseaux
