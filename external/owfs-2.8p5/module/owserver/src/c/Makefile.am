# $Id: Makefile.am,v 1.29 2010/04/09 02:43:08 alfille Exp $

bin_PROGRAMS = owserver
owserver_SOURCES = owserver.c    \
                   from_client.c \
                   to_client.c   \
                   read.c        \
                   write.c       \
                   dir.c         \
                   dirall.c      \
                   dirallslash.c \
                   data.c        \
		   error.c       \
                   handler.c     \
                   loop.c        \
                   ping.c
                   
owserver_DEPENDENCIES = ../../../owlib/src/c/libow.la

AM_CFLAGS = -I../include \
	-I../../../owlib/src/include \
	-L../../../owlib/src/c \
	-fexceptions \
	-Wall \
	-W \
	-Wundef \
	-Wshadow \
	-Wpointer-arith \
	-Wcast-qual \
	-Wcast-align \
	-Wstrict-prototypes \
	-Wredundant-decls \
	${EXTRACFLAGS} \
	${PTHREAD_CFLAGS} \
	${LIBUSB_CFLAGS}

LDADD = -low ${LIBUSB_LIBS} ${PTHREAD_LIBS} ${LD_EXTRALIBS} ${OSLIBS}

clean-generic:

	@RM@ -f *~ .*~
