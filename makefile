COMPIL=g++ -Wall -I $(OBJCPATH) -I $(SRCPATH) -I $(INCLUDEPATH) -I $(BINPATH)
OBJCPATH = ./obj
BINPATH = ./bin
SRCPATH = ./src
INCLUDEPATH = ./include
TESTPATH = ./tests


all : $(BINPATH) $(OBJCPATH) $(BINPATH)/ServeurTest $(BINPATH)/ClientTest

$(BINPATH):
	mkdir -p $(BINPATH)
	@echo "La création du fichier bin à eu lieu avec succès\n"

$(OBJCPATH):
	mkdir -p $(OBJCPATH)
	@echo "La création du fichier obj à eu lieu avec succès\n"


$(BINPATH)/ServeurTest: $(TESTPATH)/ServeurTest.cpp $(SRCPATH)/TCP.cpp $(INCLUDEPATH)/TCP.h
	@echo "Compilation de ServeurTest"
	$(COMPIL) -o $(BINPATH)/ServeurTest $(TESTPATH)/ServeurTest.cpp $(SRCPATH)/TCP.cpp
	@echo "ServeurTest compilé avec succès\n"


$(BINPATH)/ClientTest: $(TESTPATH)/ClientTest.cpp $(SRCPATH)/TCP.cpp $(INCLUDEPATH)/TCP.h
	@echo "Compilation de ClientTest"
	$(COMPIL) -o $(BINPATH)/ClientTest $(TESTPATH)/ClientTest.cpp $(SRCPATH)/TCP.cpp
	@echo "ClientTest compilé avec succès\n"



clean:
	rm -rf $(BINPATH) $(OBJCPATH)
	@echo "Nettoyage terminé\n"