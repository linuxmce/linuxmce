# Makefile for bootsplash utilities. 
# written by Stefan Reinauer <stepan@suse.de> in 2003
#

CC       = gcc
STRIP	 = strip
INSTALL  = install -s

# Triggering lib64 path from spec file.
PREFIX   ?= /usr
LIB      ?= lib

LIBDIR   = $(PREFIX)/$(LIB)

PROGRAMS        = splash splashpbm fbresolution fbtruetype fbmngplay 
PROGRAMS_STATIC = fbtruetype.static fbmngplay.static

OBJECTS_FT2  = fbtruetype.o fbtruetype-messages.o console.o ttf.o luxisri.o
OBJECTS_MNG  = fbmngplay.o fbmngplay-messages.o mng.o console.o 

LDFLAGS  =  -L$(LIBDIR)

LIBSS_FT2    = $(LIBDIR)/libfreetype.a $(LIBDIR)/libz.a -lm
LIBSD_FT2    = -lfreetype -lm

LIBSS_MNG    = $(LIBDIR)/libmng-mini.a $(LIBDIR)/libz.a -lm
LIBSD_MNG    = -lmng -lz -lm

CFLAGS   = -Os -Wall -I/usr/include/freetype2

LINKS    = $(CC) $(CFLAGS) $(LDFLAGS) -static -o $@
LINKD    = $(CC) $(CFLAGS) $(LDFLAGS) -o $@

all: $(PROGRAMS) strip

.SUFFIXES: .c .o

mostlyclean:
	-rm -f *.o core *~
clean:	mostlyclean
	-rm -f $(PROGRAMS) $(PROGRAMS_STATIC)

fbtruetype: $(OBJECTS_FT2) 
	@rm -f fbtruetype
	$(LINKD) $(LDFLAGS) $(OBJECTS_FT2) $(LIBSD_FT2)
fbtruetype.static: $(OBJECTS_FT2)
	@rm -f fbtruetype.static
	$(LINKS) $(LDFLAGS) $(OBJECTS_FT2) $(LIBSS_FT2)

fbmngplay: $(OBJECTS_MNG) 
	@rm -f fbmngplay
	$(LINKD) $(LDFLAGS) $(OBJECTS_MNG) $(LIBSD_MNG)
fbmngplay.static: $(OBJECTS_MNG)
	@rm -f fbmngplay.static
	$(LINKS) $(LDFLAGS) $(OBJECTS_MNG) $(LIBSS_MNG)

strip: $(PROGRAMS)
	$(STRIP) -s $(PROGRAMS)

install: $(PROGRAMS)
	$(INSTALL) splash "$(DESTDIR)/usr/sbin"
	$(INSTALL) fbresolution fbtruetype fbmngplay splashpbm "$(DESTDIR)/sbin"

.PHONY: mostlyclean clean fbtruetype fbtruetype.static all strip
