# $Id: Makefile.am,v 1.17 2008/09/30 23:43:56 alfille Exp $

COMMON_OWSHELL_SOURCE = ow_opt.c   \
               ow_help.c    \
               ow_server.c  \
               ow_net.c     \
               ow_browse.c  \
               ow_dl.c      \
               ow_dnssd.c   \
               ow_tcp_read.c\
               getaddrinfo.c\
               getopt.c     \
               globals.c

bin_PROGRAMS = owget owdir owread owwrite owpresent
owget_SOURCES = ${COMMON_OWSHELL_SOURCE} \
               owget.c

owdir_SOURCES = ${COMMON_OWSHELL_SOURCE} \
               owdir.c

owread_SOURCES = ${COMMON_OWSHELL_SOURCE} \
               owread.c

owwrite_SOURCES = ${COMMON_OWSHELL_SOURCE} \
               owwrite.c

owpresent_SOURCES = ${COMMON_OWSHELL_SOURCE} \
               owpresent.c

AM_CFLAGS = -I../include \
	-I../../../owlib/src/include \
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
	${EXTRACFLAGS}

LDADD = ${LD_EXTRALIBS} ${OSLIBS}

clean-generic:

	@RM@ -f *~ .*~
