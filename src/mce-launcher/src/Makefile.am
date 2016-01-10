## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/mce-launcher/glade
glade_DATA = mce-launcher.glade

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(PACKAGE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = mce-launcher

mce_launcher_SOURCES = \
	main.c        \
	listener.c        \
	listener.h 

mce_launcher_LDFLAGS = 

mce_launcher_LDADD = $(PACKAGE_LIBS)

EXTRA_DIST = $(glade_DATA)
