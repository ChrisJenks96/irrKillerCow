#Put makefile in 'PSP' root folder
#PSP irrlicht lib should be in the 'PSPSDK/LIB' folder

PSPSDK=$(shell psp-config --pspsdk-path)

TARGET = killercow
OBJS = main.o BigEnemy.o Enemy.o EnemyOrb.o Player.o

INCDIR = C:\sdk\irrlicht-1.8.4\include
CFLAGS = -G0 -Wall -O2
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lIrrlicht -lSDL -lpspirkeyb -lpspgum -lpspgu -lGL -lGLU -lm -lpsppower \
	-lfreetype -ljpeg -lpng -lpsphprm -lpspaudio -lstdc++ -lpspvfpu -lpsprtc 

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Killer Cows

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak