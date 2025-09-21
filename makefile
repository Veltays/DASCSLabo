.SILENT:
COMPIL=g++ -Wall -I $(OBJCPATH) -I $(SRCPATH) -I $(INCLUDEPATH) -I $(BINPATH)
OBJCPATH = ./obj
BINPATH = ./bin
SRCPATH = ./src
INCLUDEPATH = ./include



all : $(BINPATH) $(OBJCPATH)

$(BINPATH):
	mkdir -p $(BINPATH)
	@echo "La création du fichier bin à eu lieu avec succès\n"

$(OBJCPATH):
	mkdir -p $(OBJCPATH)
	@echo "La création du fichier obj à eu lieu avec succès\n"


$(BINPATH)/serveur: $(SRCPATH)/serveur.cpp
@echo "Création du serveur"
$(COMPIL) -o $(BINPATH)/serveur $(SRCPATH)/serveur.cpp
@echo "La création de publicité à été effectué avec succès \n"