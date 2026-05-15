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
