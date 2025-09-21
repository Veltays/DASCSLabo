#ifndef TCP_H
#define TCP_H

#define TAILLE_MAX_DATA 10000

/* Opaque forward declaration pour éviter d'inclure <netdb.h> ici */
struct addrinfo;

/* ====== Création & écoute serveur ====== */
int  CreationSocket(void);
struct addrinfo* creationAdresse(int portSer);
void bindServeurToSocket(int sEcoute, struct addrinfo* results);
void ListenOnSocket(int sEcoute);
int  ServerSocket(int portSer);

/* ====== Acceptation de connexion ====== */
int  Accept(int sEcoute, char* ipClient);
int  AcceptClient(int sEcoute);
void GetClientIP(int sService, char* ipClient);

/* ====== Côté client ====== */
int  CreationSocketClient(char* port);
struct addrinfo* ConstructionAdresseClient(char* ipServeur, char* port);
void ConnectToServer(int sClient, struct addrinfo* results);
int  ClientSocket(char* ipServeur, int portServeur);

/* ====== Envoi / Réception ====== */
int  Send(int sSocket, char* data, int taille);
int  Receive(int sSocket, char* data);

#endif 