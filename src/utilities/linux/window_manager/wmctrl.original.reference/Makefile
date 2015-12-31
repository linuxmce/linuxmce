CC       = gcc
CFLAGS   = -g -O2 -Wall -I/usr/X11R6/include
CPPFLAGS = -I/usr/X11R6/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
DEFS     = -DPACKAGE=\"wmctrl\" -DVERSION=\"1.07\" 
LIBS     = -L/usr/X11R6/lib -lX11 -lXmu -lglib-2.0

PROGRAMS = wmctrl
OBJECTS = main.o
SOURCES = main.c

all: $(PROGRAMS)

.SUFFIXES:
.SUFFIXES: .c .o

clean: 
	@rm -f $(PROGRAMS) *.o *.d core *.core

.c.o:
	$(CC) $(DEFS) $(CPPFLAGS) $(CFLAGS) -MT $@ -MD -MP -c -o $@ $<

$(PROGRAMS): $(OBJECTS)
	@rm -f $(PROGRAMS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)
