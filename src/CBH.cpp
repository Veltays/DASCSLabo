#include "../include/CBH.h"
#include "../include/TCP.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER;
int clients[NB_MAX_CLIENTS];
int nbClients = 0;


bool CBH(char *requete, char *reponse, int socket)
{
    // ***** Récupération nom de la requete *****************
    char *ptr = strtok(requete, "#");



    // ***** LOGIN ******************************************
    if (strcmp(ptr, "LOGIN") == 0)
    {
        char user[50], password[50];
        strcpy(user, strtok(NULL, "#"));
        strcpy(password, strtok(NULL, "#"));
        printf("\t[THREAD %p] LOGIN de %s\n", pthread_self(), user);
        if (estPresent(socket) >= 0) // client déjà loggé
        {
            sprintf(reponse, "LOGIN#ko#Client déjà loggé !");
            return false;
        }
        else
        {
            if (CBH_Login(user, password))
            {
                sprintf(reponse, "LOGIN#ok");
                ajoute(socket);
            }
            else
            {
                sprintf(reponse, "LOGIN#ko#Mauvais identifiants !");
                return false;
            }
        }
    }


    // ***** LOGOUT *****************************************
    if (strcmp(ptr, "LOGOUT") == 0)
    {
        if(CBH_Logout(socket) == false)
        {
                
        return false;
        }
        sprintf(reponse, "LOGOUT#ok");

    }

    return true;
}





bool CBH_Login(const char *user, const char *password)
{
    if (strcmp(user,"wagner")==0 && strcmp(password,"abc123")==0) 
    {
        return true;
    }
    if (strcmp(user,"charlet")==0 && strcmp(password,"xyz456")==0) 
    {
        return true;
    }
    return false;
}



bool CBH_Logout(int socket)
{
        printf("\t[THREAD %p] LOGOUT\n", pthread_self());
        retire(socket);
        return false;
}




//***** Gestion de l'état du protocole ******************************
int estPresent(int socket)
{
    int indice = -1;
    pthread_mutex_lock(&mutexClients);
    for (int i = 0; i < nbClients; i++)
        if (clients[i] == socket)
        {
            indice = i;
            break;
        }
    pthread_mutex_unlock(&mutexClients);
    return indice;
}


void ajoute(int socket)
{
    pthread_mutex_lock(&mutexClients);
    clients[nbClients] = socket;
    nbClients++;
    pthread_mutex_unlock(&mutexClients);
}


void retire(int socket)
{
    int pos = estPresent(socket);
    if (pos == -1)
        return;
    pthread_mutex_lock(&mutexClients);
    for (int i = pos; i <= nbClients - 2; i++)
        clients[i] = clients[i + 1];
    nbClients--;
    pthread_mutex_unlock(&mutexClients);
}





void CBH_Close()
{
    pthread_mutex_lock(&mutexClients);
    for (int i = 0; i < nbClients; i++)
        close(clients[i]);
    pthread_mutex_unlock(&mutexClients);
}











// bool CBH_Get_Specialites(char *reponse)
// {
//     return true;
// }

// bool CBH_Get_Doctors(char *reponse)
// {
//     return true;
// }

// bool CBH_Search_Consultations(char *specialiter, char *medecin, char *dateDebut, char *dateFin)
// {
//     return true;
// }

// bool CBH_BOOK_CONSULTATION(int idConsultation, int idPatient, char *reponse)
// {
//     return true;
// }