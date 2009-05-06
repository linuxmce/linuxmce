# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = lmce_launch_manager.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/lmce_launch_manager.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = lmce_launch_manager

# the application source, library search path, and link libraries
lmce_launch_manager_SOURCES = Launch_Manager.cpp Launch_ManagerBase.cpp \
	lmce_launch_manager.cpp lmce_launch_managerwidget.cpp lmce_launch_managerwidgetbase.ui main.cpp
lmce_launch_manager_LDFLAGS = $(KDE_RPATH) $(all_libraries)
lmce_launch_manager_LDADD = -lSerializeClass -lPlutoUtils -lDCECommon \
	$(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = lmce_launch_manager.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/lmce_launch_manager
shellrc_DATA = lmce_launch_managerui.rc

AM_CXXFLAGS = -fexceptions

all-local:
	cp $(bin_PROGRAMS) $(top_srcdir)/../bin/
