#include "TCP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // close()
#include <arpa/inet.h> // inet_ntop, htons
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int ServerSocket(int port)
{

    //Structure pour filtrer pour le getaddrinfo
    struct addrinfo hints;

    //Structure pour les résultats du getaddrinfo
    struct addrinfo *results;

    char portStr[10];

    // Convertir le port en string pour le getaddrinfo
    snprintf(portStr, sizeof(portStr), "%d", port);

    //Création du socket :
    //AF_INET : IPV4
    //SOCK_STREAM : TCP

    int serverSocket;
    int domaine = AF_INET;
    int TCP = SOCK_STREAM;

    if ((serverSocket = socket(domaine, TCP, 0)) == -1)
    {
        perror("Erreur de socket()");
        exit(1);
    }

    //On initialise tous les champs de la structure a 0
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // Pour bind()

    if (getaddrinfo(NULL, portStr, &hints, &results) != 0)
    {
        perror("getaddrinfo");
        return -1;
    }

    // Liaison de la socket à l'adresse
    if (bind(serverSocket, results->ai_addr, results->ai_addrlen) < 0)
    {
        perror("Erreur de bind()");
        freeaddrinfo(results);
        exit(1);
    }

    // En attente d'une connexion : SOMAXCONN (pour dire que la file peut attendre infini de connexion)
    if (listen(serverSocket, SOMAXCONN) == -1)
    {
        perror("Erreur de listen()");
        freeaddrinfo(results);
        exit(1);
    }
    printf("listen() reussi !\n");

    freeaddrinfo(results);

    return serverSocket;
}

int Accept(int serverSocket, char *ipClient)
{
    int sService;

    if ((sService = accept(serverSocket, NULL, NULL)) == -1)
    {
        perror("Erreur de accept()");
        exit(1);
    }

    printf("accept() reussi !\n");
    printf("socket de service = %d\n", sService);

    struct sockaddr_in adrClient; //Pour stocker une adresse IPV4
    socklen_t adrClientLen = sizeof(struct sockaddr_in);

    char host[NI_MAXHOST];
    char port[NI_MAXSERV];

    getpeername(sService, (struct sockaddr *)&adrClient, &adrClientLen); //remplit adrClient (IP + port du client)

    getnameinfo((struct sockaddr *)&adrClient, adrClientLen, //convertit adrClient en host (IP texte) et port (texte)
                host, NI_MAXHOST, port, NI_MAXSERV,
                NI_NUMERICSERV | NI_NUMERICHOST);

    printf("Client connecte --> Adresse IP: %s -- Port: %s\n", host, port);

    //Pour la zone mémoire
    if (ipClient != NULL)
    {
        strncpy(ipClient, host, NI_MAXHOST);
        ipClient[NI_MAXHOST - 1] = '\0';
    }

    return sService;
}

int ClientSocket(char *ipServeur, int portServeur)
{
    struct addrinfo hints;

    struct addrinfo *results;

    char portStr[10];

    // Convertir le port en string pour le getaddrinfo
    snprintf(portStr, sizeof(portStr), "%d", portServeur);

    int clientSocket;
    int domaine = AF_INET;
    int TCP = SOCK_STREAM;

    if ((clientSocket = socket(domaine, TCP, 0)) == -1)
    {
        perror("Erreur de socket()");
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_NUMERICSERV;

    if (getaddrinfo(ipServeur, portStr, &hints, &results) != 0)
    {
        perror("getaddrinfo");
        return -1;
    }

    // Demande de connexion
    if (connect(clientSocket, results->ai_addr, results->ai_addrlen) == -1)
    {
        perror("Erreur de connect()");
        freeaddrinfo(results);
        close(clientSocket);
        exit(1);
    }

    freeaddrinfo(results);

    return clientSocket;
}

int Send(int sSocket, char *data, int taille)
{
    int bytesEnvoyes;
    char temp[1024]; // entête + message
    int entete = 4;

    // Créer l’entête
    sprintf(temp, "%04d", taille);

    // Copier les données brutes juste après
    memcpy(temp + entete, data, taille);

    //temp = un string de tout et entete + taille c'est la taille de tout
    bytesEnvoyes = send(sSocket, temp, entete + taille, 0);
    if (bytesEnvoyes == -1)
    {
        perror("Erreur lors de l'envoi");
        return -1;
    }

    return bytesEnvoyes;
}

int Receive(int sSocket, char *data)
{
    int bytesRecus;
    char header[4];   // pour les 4 chiffres + '\0'
    int tailleMsg;

    //On lit l'entete donc 4 octets
    bytesRecus = recv(sSocket, header, 4, 0);
    if (bytesRecus <= 0)
    {
        perror("Erreur lecture entête");
        return -1;
    }

    header[4] = '\0';           // terminer la chaîne
    tailleMsg = atoi(header);   // convertir "0014" → 14

    //On envoie le message
    bytesRecus = recv(sSocket, data, tailleMsg, 0);
    if (bytesRecus <= 0)
    {
        perror("Erreur lecture message");
        return -1;
    }

    data[tailleMsg] = '\0'; // assurer fin de chaîne

    return bytesRecus;
}