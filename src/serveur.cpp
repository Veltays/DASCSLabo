#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // pour memset
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
int main()
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
        
    int sEcoute;
    printf("pid = %d\n", getpid());
    // Creation de la socket
    if ((sEcoute = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Erreur de socket()");
        exit(1);
    }
    printf("socket creee = %d\n", sEcoute);



    /* ---------------------------
    ? Construction de l'adresse 
    * On déclare une structure addrinfo pour les hints (indications) -> On lui donnera des informations sur le type d'adresse que l'on veut récupérer
    * On déclare un pointeur de structure addrinfo pour les résultats -> On récupérera le resultat de getaddrinfo dans cette structure
    * On initialise la structure hints à 0 avec memset
    * On remplit la structure hints
        - ai_family = AF_INET (on veut une adresse IPv4)
        - ai_socktype = SOCK_STREAM (on veut une socket de type flux de données) (TCP)
        - ai_flags = AI_PASSIVE | AI_NUMERICSERV 
            - AI_PASSIVE : pour une connexion passive (serveur) -> on précise qu'on va ecouter sur toute les interfaces (toute les adresses IP qui essaient de se connecter)
            - AI_NUMERICSERV : le port est donné sous forme numérique (50000)
        
    * On appelle la fonction getaddrinfo pour construire l'adresse
        - 1er argument : NULL (on veut écouter sur toutes les interfaces) -> d'ou le AI_PASSIVE  ( revient a dire 0.0.0.0)
        - 2ème argument : "50000" (le port sur lequel on veut écouter) 
        - 3ème argument : &hints (les indications sur le type d'adresse que l'on veut)
        - 4ème argument : &results (le pointeur vers la structure où l'on va récupérer le résultat)
        ! On vérifie que getaddrinfo s'est bien passé (si != 0)
            - si erreur on quitte le programme avec exit(1)


    * On déclare des tableaux de caractères pour stocker
        - NI_MAXHOST : l'adresse IP
        - NI_MAXSERV : le port

    * On appelle la fonction getnameinfo pour afficher l'adresse IP et le port
        - 1er argument : results->ai_addr (l'adresse obtenue par getaddrinfo)
        - 2ème argument : results->ai_addrlen (la taille de l'adresse)
        - 3ème argument : host (le tableau pour stocker l'adresse IP)
        - 4ème argument : NI_MAXHOST (la taille du tableau pour l'adresse IP)
        - 5ème argument : port (le tableau pour stocker le port)
        - 6ème argument : NI_MAXSERV (la taille du tableau pour le port)
        - 7ème argument : NI_NUMERICSERV | NI_NUMERICHOST (pour afficher l'adresse et le port sous forme numérique)
    
    * On affiche l'adresse IP et le port
    ------------------------------ */ 


    // Construction de l'adresse
    struct addrinfo hints;
    struct addrinfo *results;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; // pour une connexion passive

    if (getaddrinfo(NULL, "50000", &hints, &results) != 0)
        exit(1);


    // Affichage du contenu de l'adresse obtenue
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
    getnameinfo(results->ai_addr, results->ai_addrlen,
                host, NI_MAXHOST, port, NI_MAXSERV,
                NI_NUMERICSERV | NI_NUMERICHOST);
    printf("Mon Adresse IP: %s -- Mon Port: %s\n", host, port);



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



    // Liaison de la socket à l'adresse
    if (bind(sEcoute, results->ai_addr, results->ai_addrlen) < 0)
    {
        perror("Erreur de bind()");
        exit(1);
    }
    freeaddrinfo(results);
    printf("bind() reussi !\n");
    


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

    /* ---------------------------
    ? Lecture sur la socket
    * On déclare un int pour le nombre d'octets lus
    * On déclare un tableau de caractères pour stocker les données lues
    * On tente de lire des données avec read
        - 1er argument : sService (le descripteur de fichier de la socket de service)
        - 2ème argument : buffer1 (le tableau pour stocker les données lues)
        - 3ème argument : 5 (le nombre d'octets à lire)
        ! On vérifie que read s'est bien passé (si -1)
            - si erreur on affiche un message d'erreur avec perror
            - on ferme les sockets avec close
            - on quitte le programme avec exit(1)
    ------------------------------ */ 


    int nb;
    char buffer1[50];
    if ((nb = read(sService, buffer1, 5)) == -1)
    {
        perror("Erreur de read()");
        close(sEcoute);
        close(sService);
    }
    buffer1[nb] = 0;
    printf("nbLus = %d Lu: --%s--\n", nb, buffer1);




    /* ---------------------------
    ? Ecriture sur la socket
    * On déclare un tableau de caractères pour stocker la réponse
    * On remplit le tableau avec la réponse (on ajoute un préfixe [SERVEUR] au message reçu)
    * On tente d'écrire des données avec write
        - 1er argument : sService (le descripteur de fichier de la socket de service)
        - 2ème argument : reponse (le tableau contenant la réponse)
        - 3ème argument : strlen(reponse) (la taille de la réponse)
        ! On vérifie que write s'est bien passé (si -1)
            - si erreur on affiche un message d'erreur avec perror
            - on ferme les sockets avec close
            - on quitte le programme avec exit(1)
    * On affiche le nombre d'octets écrits et la réponse envoyée
    ------------------------------ */ 


    char reponse[50];
    sprintf(reponse, "[SERVEUR] %s", buffer1);
    if ((nb = write(sService, reponse, strlen(reponse))) == -1)
    {
        perror("Erreur de write()");
        close(sEcoute);
        close(sService);
    }
    printf("nbEcrits = %d Ecrit: --%s--\n", nb, reponse);


    /* ---------------------------
    ? Fermeture de la connexion cote serveur
    * On ferme les sockets avec close
    * On quitte le programme avec exit(0)
    ------------------------------ */ 


    close(sService);
    close(sEcoute);
    exit(0);
}