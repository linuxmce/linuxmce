# $Id: Makefile.am,v 1.21 2008/06/24 11:06:47 d1mag Exp $

bin_PROGRAMS = owftpd
owftpd_SOURCES = file_list.c \
                 file_cd.c    \
                 ftp_command.c \
                 ftp_listener.c \
                 ftp_session.c   \
                 owftpd.c         \
                 telnet_session.c  \
                 watchdog.c         \
                 daemon_assert.c
owftpd_DEPENDENCIES = ../../../owlib/src/c/libow.la

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
	${LIBUSB_CFLAGS} \
	${PTHREAD_CFLAGS}

LDADD = -low ${LIBUSB_LIBS} ${PTHREAD_LIBS} ${LD_EXTRALIBS} ${OSLIBS}


clean-generic:

	@RM@ -f *~ .*~
