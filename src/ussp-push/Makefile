#
# UNrooted.net example code
#
# Minimal Makefile used to build my Bluez OBEX example.  The generated 
# executable 'ussp-push' can be used to push files using OBEX over RFCOMM with
# the Bluez Linux Bluetooth stack.  You should be able to just 'make' directly
# without any setup.  Check the GLIB and OBEX defines just below this comment
# if you're having trouble with the compile.
#


#
# On most systems glib comes with a helper program used to determine what 
# cflags and libraries to use when compiling a program that uses glib.  These
# definitions should work on most systems, but if they don't on yours you
# should have little trouble figuring out what they should be.
#

GLIBINC=$(shell glib-config --cflags)
GLIBLIB=$(shell glib-config --libs)


#
# These are the default paths the openobex package installs under.  Hack as
# necessary to reflect your setup.
#

OBEXINC=-I/usr/local/include
OBEXLIB=-L/usr/local/lib -lopenobex


#
# The make rules.. basically just two files here plus the openobex and glib
# libraries and we can make the executable.
#

all: ussp-push

ussp-push: obex_main.o obex_socket.o
	gcc obex_main.o obex_socket.o ${GLIBLIB} ${OBEXLIB} -o ussp-push
	cp ussp-push ../bin
 
obex_main.o: obex_main.c obex_socket.h
	gcc ${OBEXINC} ${GLIBINC} -c obex_main.c -o obex_main.o

obex_socket.o: obex_socket.c obex_socket.h
	gcc ${OBEXINC} ${GLIBINC} -c obex_socket.c -o obex_socket.o

clean:
	rm -f *.o ussp-push
