#Put makefile in 'PSP' root folder
#PSP irrlicht lib should be in the 'PSPSDK/LIB' folder

IDIR = C:\sdk\irrlicht-1.8.4\include
CC= psp-g++ -Wall
CFLAGS= -I$(IDIR)

ODIR= obj
LDIR = 

LIBS= -lIrrlicht -lpspgum -lpspgu -lpspgl -lm -lpsprtc -lstdc++

all: KillerCows

KillerCows: main.o BigEnemy.o Enemy.o EnemyOrb.o Player.o
	$(CC) main.cpp BigEnemy.cpp Enemy.cpp EnemyOrb.cpp Player.cpp -o KillerCows -I$(IDIR) $(LDIR) $(LIBS)
	sudo cp libfmod.so.11 /usr/lib/

main.o: main.cpp
	$(CC) -c main.cpp -I$(IDIR)

BigEnemy.o: BigEnemy.cpp
	$(CC) -c BigEnemy.cpp -I$(IDIR)

Enemy.o: Enemy.cpp
	$(CC) -c Enemy.cpp -I$(IDIR)

EnemyOrb.o: EnemyOrb.cpp
	$(CC) -c EnemyOrb.cpp -I$(IDIR)

Player.o: Player.cpp
	$(CC) -c Player.cpp -I$(IDIR)

clean:
	rm -rf *o KillerCows