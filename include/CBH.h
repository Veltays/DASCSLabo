#ifndef CBH_H
#define CBH_H
#define NB_MAX_CLIENTS 20

bool CBH(char* requete, char* reponse,int socket);
bool CBH_Login(const char* user,const char* password);
bool CBH_Logout(int socket);

int estPresent(int socket);
void ajoute(int socket);
void retire(int socket);

bool CBH_Get_Specialites(char* reponse);
bool CBH_Get_Doctors(char* reponse);
bool CBH_Search_Consultations(char* specialiter,char* medecin,char* dateDebut, char*dateFin);
bool CBH_BOOK_CONSULTATION(int idConsultation,int idPatient,char* reponse);
void CBH_Close();


#endif // CBH_H