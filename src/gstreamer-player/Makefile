CC=gcc

GST_LIBS   = $(shell pkg-config --libs gstreamer-0.10 gstreamer-interfaces-0.10)
GST_CFLAGS = $(shell pkg-config --cflags gstreamer-0.10 gstreamer-interfaces-0.10)
GTK_LIBS   = $(shell pkg-config --libs gtk+-2.0)
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-2.0)

local_LIB = @mkdir .libs

MESSAGE_CC    = @echo '  CC..... '$@;
MESSAGE_LINK  = @echo '  LINK... '$@;
MESSAGE_CLEAN = @echo '  CLEAN.. '$@;

DESTDIR = /usr/bin

CFLAGS= -g -Wall

Xine_Player: Xine_Player.o gsbackend.o gsfrontend.o
	$(MESSAGE_LINK) $(CC) $(GTK_LIBS)  $(GST_LIBS) -o $@ $^
	$(local_LIB)
	mv *.o .libs

%.o:: %.c
	$(MESSAGE_CC) $(CC) $(GST_CFLAGS) $(GTK_CFLAGS) -o $@ -c $<

bin += $(Xine_Player)

install: $(bin)
	install -m 777 Xine_Player $(DESTDIR)
clean:
	rm -rf Xine_Player
	rm -rf Xine_Player.o 
	rm -rf gsbackend.o
	rm -rf gsfrontend.o
	rm -rf .libs
