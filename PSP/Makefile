#Put makefile in 'PSP' root folder
#PSP irrlicht lib should be in the 'PSPSDK/LIB' folder

PSPSDK=$(shell psp-config --pspsdk-path)

TARGET = killercow
OBJS = main.o BigEnemy.o Enemy.o EnemyOrb.o Player.o

INCDIR =
CFLAGS = -G0 -w -O2
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lengine -lSDL -lpspirkeyb -lpspgum -lpspgu -lglut -lGLU -lGL  -lm -lpsppower -lfreetype -ljpeg -lpng -lpsphprm  -lpspaudiocodec -lpspaudiolib -lpspaudio -lstdc++ -lpspvfpu -lpsprtc

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Killer Cows
PSP_LARGE_MEMORY = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak