# Conception, analyse et comparaison de différents modèles de serveurs TCP en C

## **1. Objectif**

L'objectif de cette expérience est de démontrer et de comparer les différents modèles de serveurs TCP en C. Dans ce rapport, le modèle itératif, concurrent avec fork, pthreads, et les multiplexage I/O seront particulièrement discutés. Le serveur sera exécuté en tant que daemon et les logs seront gérés en utilisant syslog.

## **2. Modèle itérative**

### **2.1 Principe**

Dans le modèle itératif, les clients seront servis un par un et dans l'ordre des connexions. Par conséquent, la réponse de la requête des clients seront bloqués par les précédents.

### **2.2 Démonstration**

#### **2.2.1 Test sur un seul client**
Après compilation, le serveur est exécuté en utilisant la commande `./server`. Dans un autre terminal ou onglets, on lance les clients en utilisant `netcat`.

Terminal du serveur:

```bash
$ ./serveur
Serveur itératif démarré sur le port 9999...
```

Voici ce à quoi le terminal du client doit ressembler:

```bash
# client 1
$ nc 127.0.0.1 9999
hello
[Connexion #1]: Echo hello
```

#### **2.2.2 Cas de deux clients**

Si on essaie de connecter deux clients en même temps, il n'y aura aucun problème et les deux clients seront connectés sur le serveur. Mais si le deuxième client envoie un message au serveur alors que le premier ne l'a pas encore fait, ce dernier ne recevera pas l'echo.
