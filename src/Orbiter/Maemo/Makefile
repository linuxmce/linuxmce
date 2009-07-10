VERSION=0710
PACKAGE_VERSION=05
OS=os2008
DESTDIR=build
CC=gcc

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES -I/opt/libxine1-pluto/include -I/opt/libsdl1.2-1.2.7+1.2.8cvs20041007/include -I/opt/libsdl1.2-1.2.7+1.2.8cvs20041007/include/SD

CFLAGS=`pkg-config --cflags --libs gconf-2.0 hildon-1 gtk+-2.0 libosso gdk-2.0 conic` $(SNR_CPPFLAGS)
#CFLAGS=`pkg-config --cflags --libs gconf-2.0 hildon-libs hildon-fm gtk+-2.0 libosso gdk-2.0 conic` $(SNR_CPPFLAGS)
#CFLAGS=1`pkg-config --cflags gtk+-2.0 hildon-1`
LDFLAGS=
INCLUDE=-I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
#TARGET=plutohomeorbiter
 
pluto: 
	
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -o plutohomeorbiter orbiter.c
	
	strip plutohomeorbiter

lmce: 
	
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -o linuxmceorbiter orbiter.c
	
	strip linuxmceorbiter	


clean:
	-rm -f linuxmceorbiter plutohomeorbiter


