#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // pour memset
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "../include/TCP.h"

char host[NI_MAXHOST];
char port[NI_MAXSERV];
int sEcoute;

#pragma region CreationSocket

int CreationSocket()
{

    /* ---------------------------
    ? Construction du socket
    * on déclare un socket d'écoute (serveur)
         * on déclare un int pour la socket d'écoute ( il va récupérer un descripteur de fichier)
         * on affiche le PID du processus
         * on crée la socket avec la fonction socket (AF_INET, SOCK_STREAM, 0)
             - AF_INET : famille d'adresse IPv4
             - SOCK_STREAM : type de socket (flux de données) (TCP)
             - 0 : protocole (0 pour choisir le protocole par défaut) (il va mettre TCP car SOCK_STREAM)
         * on vérifie que la création de la socket s'est bien passée (si -1)
             - si erreur on affiche un message d'erreur avec perror
             - on quitte le programme avec exit(1)
    on affiche le descripteur de fichier de la socket
    ------------------------------ */

    printf("pid = %d\n", getpid());
    // Creation de la socket
    if ((sEcoute = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Erreur de socket()");
        exit(1);
    }

    int opt = 1;
    // Active SO_REUSEADDR pour pouvoir réutiliser le port rapidement   --> permet d'éviter l'erreur "Address already in use" lors du redémarrage rapide du serveur
    if (setsockopt(sEcoute, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    printf("socket creee = %d\n", sEcoute);
    return sEcoute;
}

struct addrinfo *creationAdresse(int portSer)
{
    struct addrinfo hints;
    struct addrinfo *results = NULL;

    // convertir le port en chaîne numérique (AI_NUMERICSERV l'exige)
    char portStr[16];
    snprintf(portStr, sizeof(portStr), "%d", portSer);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;                    // IPv4
    hints.ai_socktype = SOCK_STREAM;              // TCP
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; // écoute sur toutes interfaces, port numérique

    int ret = getaddrinfo(NULL, portStr, &hints, &results);
    if (ret != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(1);
    }

    // Affichage IP/port sous forme numérique

    if (getnameinfo(results->ai_addr, results->ai_addrlen,
                    host, sizeof(host),
                    port, sizeof(port),
                    NI_NUMERICHOST | NI_NUMERICSERV) == 0)
    {
        printf("Mon Adresse IP: %s -- Mon Port: %s\n", host, port);
    }

    return results; // à libérer plus tard avec freeaddrinfo(results)
}

void bindServeurToSocket(int sEcoute, struct addrinfo *results)
{
    /* ---------------------------
    ? Liaison de la socket à l'adresse
    * on tente de lier la socket à l'adresse avec bind
        - 1er argument : sEcoute (le descripteur de fichier de la socket)
        - 2ème argument : results->ai_addr (l'adresse obtenue par getaddrinfo)
        - 3ème argument : results->ai_addrlen (la taille de l'adresse)
        ! On vérifie que bind s'est bien passé (si < 0)
            - si erreur on affiche un message d'erreur avec perror
            - on quitte le programme avec exit(1)
    * on libère la mémoire allouée pour les résultats de getaddrinfo avec free
    * on affiche un message pour indiquer que bind a réussi
    ------------------------------ */

    if (bind(sEcoute, results->ai_addr, results->ai_addrlen) < 0)
    {
        perror("Erreur de bind()");
        exit(1);
    }
    freeaddrinfo(results);
    printf("bind() reussi !\n");
}

void ListenOnSocket(int sEcoute)
{
    /* ---------------------------
    ? Mise à l'écoute de la socket
    * On tente de mettre le socket en écoute avec listen
        - 1er argument : sEcoute (le descripteur de fichier de la socket)
        - 2ème argument : SOMAXCONN (le nombre maximum de connexions en attente) -> 128 par défaut
        ! On vérifie que listen s'est bien passé (si < 0)
            - si erreur on affiche un message d'erreur avec perror
            - on quitte le programme avec exit(1)
    * on affiche un message pour indiquer que listen a réussi
    ------------------------------ */

    // Mise à l'écoute de la socket
    if (listen(sEcoute, SOMAXCONN) == -1)
    {
        perror("Erreur de listen()");
        exit(1);
    }
    printf("listen() reussi !\n");
}

int ServerSocket(int portSer)
{
    sEcoute = CreationSocket();

    struct addrinfo *results = creationAdresse(portSer);

    bindServeurToSocket(sEcoute, results);

    ListenOnSocket(sEcoute);

    return sEcoute;
}

#pragma endregion CreationSocket

#pragma region Accept

int Accept(int sEcoute, char *ipClient)
{

    int sService = AcceptClient(sEcoute);

    GetClientIP(sService, ipClient);
}

int AcceptClient(int sEcoute)
{
    /* ---------------------------
    ? Attente d'une connexion
    * On déclare un int pour la socket de service (la socket qui va gérer la connexion avec le client)
    * On tente d'accepter une connexion avec accept
        - 1er argument : sEcoute (le descripteur de fichier de la socket d'écoute)
        - 2ème argument : NULL (on ne veut pas récupérer l'adresse du client)
        - 3ème argument : NULL (on ne veut pas récupérer la taille de l'adresse du client)
        ! On vérifie que accept s'est bien passé (si -1)
            - si erreur on affiche un message d'erreur avec perror
            - on quitte le programme avec exit(1)
    * on affiche un message pour indiquer que accept a réussi
    * on affiche le descripteur de fichier de la socket de service
    ------------------------------ */
    int sService;
    if ((sService = accept(sEcoute, NULL, NULL)) == -1)
    {
        perror("Erreur de accept()");
        exit(1);
    }
    printf("accept() reussi !\n");
    printf("socket de service = %d\n", sService);

    return sService;
}

void GetClientIP(int sService, char *ipClient)
{

    /* ---------------------------
    ? Recuperation d'information sur le client connecte
    * On déclare une structure sockaddr_in pour l'adresse du client
    * On déclare une socklen_t pour la taille de l'adresse du client (nécessaire)
    * On appelle la fonction getpeername pour récupérer l'adresse du client grâce à la socket de service
        - 1er argument : sService (le descripteur de fichier de la socket de service)
        - 2ème argument : (struct sockaddr *)&adrClient (l'adresse du client)
        - 3ème argument : &adrClientLen (la taille de l'adresse du client)

    * On appelle la fonction getnameinfo pour afficher l'adresse IP et le port du client grâce à son adresse récupérée avec getpeername
        - 1er argument : (struct sockaddr *)&adrClient (l'adresse du client
        - 2ème argument : adrClientLen (la taille de l'adresse du client)
        - 3ème argument : host (le tableau pour stocker l'adresse IP)
        - 4ème argument : NI_MAXHOST (la taille du tableau pour l'adresse IP
        - 5ème argument : port (le tableau pour stocker le port)
        - 6ème argument : NI_MAXSERV (la taille du tableau pour le port)
        - 7ème argument : NI_NUMERICSERV | NI_NUMERICHOST (pour afficher l'adresse et le port sous forme numérique)
    * On affiche l'adresse IP et le port du client
    ------------------------------ */

    struct sockaddr_in adrClient;
    socklen_t adrClientLen = sizeof(struct sockaddr_in); // nécessaire
    getpeername(sService, (struct sockaddr *)&adrClient, &adrClientLen);
    getnameinfo((struct sockaddr *)&adrClient, adrClientLen, host, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICSERV | NI_NUMERICHOST);
    printf("Client connecte --> Adresse IP: %s -- Port: %s\n", host, port);
}

#pragma endregion Accept

#pragma region ClientSocket

int CreationSocketClient(char *port)
{

    // Création de la socket
    int sClient;
    if ((sClient = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Erreur de socket()");
        return -1;
    }
    return sClient;
}

struct addrinfo *ConstructionAdresseClient(char *ipServeur, char *port)
{
    struct addrinfo hints;
    struct addrinfo *results;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_NUMERICSERV; // port numérique

    if (getaddrinfo(ipServeur, port, &hints, &results) != 0)
    {
        perror("Erreur de getaddrinfo()");
    }

    return results;
}

void ConnectToServer(int sClient, struct addrinfo *results)
{
    // Demande de connexion
    if (connect(sClient, results->ai_addr, results->ai_addrlen) == -1)
    {
        perror("Erreur de connect()");
        close(sClient);
        freeaddrinfo(results);
    }
}

int ClientSocket(char *ipServeur, int portServeur)
{

    struct addrinfo hints;
    struct addrinfo *results;
    char port[10];

    // convertir port en chaîne
    sprintf(port, "%d", portServeur);

    int sClient = CreationSocketClient(port);

    struct addrinfo *results = ConstructionAdresseClient(ipServeur, port);

    ConnectToServer(sClient, results);

    freeaddrinfo(results);

    return sClient; // socket connectée prête à l’emploi
}

#pragma endregion ClientSocket

#pragma region Send

int Send(int sSocket, char *data, int taille)
{
    int nbEcrits;
    int tailleReseau = htonl(taille); // conversion taille en format réseau

    // 1. envoyer la taille (4 octets)
    nbEcrits = write(sSocket, &tailleReseau, sizeof(int));
    if (nbEcrits != sizeof(int))
    {
        perror("Erreur d’envoi de l’entête");
        return -1;
    }

    // 2. envoyer les données utiles
    nbEcrits = write(sSocket, data, taille);
    if (nbEcrits != taille)
    {
        perror("Erreur d’envoi des données");
        return -1;
    }

    printf("Send() → taille=%d, envoyés=%d, data=--%.*s--\n", taille, nbEcrits, taille, data);
    return nbEcrits;
}

#pragma endregion Send

#pragma region Receive

int Receive(int sSocket, char *data)
{
    int nbLus;
    int tailleReseau;
    int taille;

    // 1. lire l’entête (4 octets)
    nbLus = read(sSocket, &tailleReseau, sizeof(int));
    if (nbLus == 0)
    {
        // connexion fermée proprement
        return 0;
    }
    if (nbLus != sizeof(int))
    {
        perror("Erreur de lecture de l’entête");
        return -1;
    }

    taille = ntohl(tailleReseau); // conversion réseau → machine
    if (taille > TAILLE_MAX_DATA)
    {
        fprintf(stderr, "Erreur : taille reçue (%d) > TAILLE_MAX_DATA\n", taille);
        return -1;
    }

    // 2. lire exactement "taille" octets
    int totalLus = 0;
    while (totalLus < taille)
    {
        nbLus = read(sSocket, data + totalLus, taille - totalLus);
        if (nbLus <= 0)
        {
            perror("Erreur de lecture des données");
            return -1;
        }
        totalLus += nbLus;
    }

    printf("Receive() → taille=%d, lus=%d, data=--%.*s--\n", taille, totalLus, taille, data);
    return totalLus;
}

#pragma endregion Receive