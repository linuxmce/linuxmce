LIBMNG_PREFIX = /usr/local

CC = gcc

#CFLAGS = -g -O2 -Wall \
#	`gtk-config --cflags` \
#	-I$(LIBMNG_PREFIX)/include
CFLAGS = -g -O2 -Wall \
	`gdk-pixbuf-config --cflags` \
	-I$(LIBMNG_PREFIX)/include

#LIBS = `gtk-config --libs` \
#	-L$(LIBMNG_PREFIX)/lib -lmng -ljpeg -lz
LIBS = `gdk-pixbuf-config --libs` \
	-L$(LIBMNG_PREFIX)/lib -lmng -ljpeg -llcms -lz

OBJ = dummy.o \
      gtk-mng-view.o

# GRR:  I personally prefer "gmngview" or "gtk-mng-view" over "dummy"...
#EXE_BASENAME=dummy
EXE_BASENAME=gmngview

all: $(EXE_BASENAME) $(EXE_BASENAME)-static

$(EXE_BASENAME): $(OBJ)
	$(CC) -o $(EXE_BASENAME) $(OBJ) $(LIBS)

$(EXE_BASENAME)-static: $(OBJ)
	$(CC) -static -o $(EXE_BASENAME)-static $(OBJ) $(LIBS)

.c.o: gtk-mng-view.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f core $(EXE_BASENAME) $(EXE_BASENAME)-static $(OBJ)
