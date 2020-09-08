#Put makefile in 'Linux' root folder (it assumes 'Windows' is '../Windows')
#Linux irrlicht lib should be in the 'Linux' folder where this makefile is.

IDIR = irrlicht-1.8.4/include/ -Ifmodstudioapi20011linux/api/core/inc/ -I../Windows/KillerCowAppWin/
CC= g++ -Wall
CFLAGS= -I$(IDIR)

ODIR= obj
LDIR = -Lirrlicht-1.8.4/lib/Linux/ -Lfmodstudioapi20011linux/api/core/lib/x86/

SRC_DIR = ../Windows/KillerCowAppWin

LIBS= -lIrrlicht -lGLU -lGL -lX11 -lXxf86vm -lfmod

all: KillerCows

KillerCows: main.o BigEnemy.o Enemy.o EnemyOrb.o Player.o
	$(CC) $(SRC_DIR)/main.cpp $(SRC_DIR)/BigEnemy.cpp $(SRC_DIR)/Enemy.cpp $(SRC_DIR)/EnemyOrb.cpp $(SRC_DIR)/Player.cpp -o KillerCows -I$(IDIR) $(LDIR) $(LIBS)
	sudo cp libfmod.so.11 /usr/lib/

main.o: $(SRC_DIR)/main.cpp
	$(CC) -c $(SRC_DIR)/main.cpp -I$(IDIR)

BigEnemy.o: $(SRC_DIR)/BigEnemy.cpp
	$(CC) -c $(SRC_DIR)/BigEnemy.cpp -I$(IDIR)

Enemy.o: $(SRC_DIR)/Enemy.cpp
	$(CC) -c $(SRC_DIR)/Enemy.cpp -I$(IDIR)

EnemyOrb.o: $(SRC_DIR)/EnemyOrb.cpp
	$(CC) -c $(SRC_DIR)/EnemyOrb.cpp -I$(IDIR)

Player.o: $(SRC_DIR)/Player.cpp
	$(CC) -c $(SRC_DIR)/Player.cpp -I$(IDIR)

clean:
	rm -rf *o KillerCows