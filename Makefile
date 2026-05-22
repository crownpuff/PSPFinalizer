TARGET = Finalizer
OBJS = source/main.o

CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpsputility -lpspreg -lm

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Finalizer

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
