## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/mce-installer/glade
glade_DATA = mce-installer.glade

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(PACKAGE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = mce-installer

mce_installer_SOURCES = \
	main.c         \
	StepInstallDependencies.c \
	StepInstallDependencies.h \
	Step1A.c         \
	Step1A.h         \
	common.h         \
	common.c         \
	Step1B.c         \
	Step1B.h         \
	Step1C.c         \
	Step1C.h         \
	Step1D.c         \
	Step1D.h         \
	network-common.c         \
	network-common.h         \
	Step2A.c         \
	Step2A.h         \
	Step3.c         \
	Step3.h         \
	Step2E.c         \
	Step2E.h         \
	Step2C.c         \
	Step2C.h         \
	Step4.c         \
	Step4.h         \
	Step5.c         \
	Step5.h         \
	Step0B.c         \
	Step0B.h         \
	Step2F.c         \
	Step2F.h         \
	Step0A.c         \
	Step0A.h         \
	Step6.c         \
	Step6.h 	\
	StepAptMirror.h \
	StepAptMirror.c \
	StepUI.h \
	StepUI.c \
	StepUbuntuExtraCD.c \
	StepUbuntuExtraCD.h \
	StepLinuxMceCD.c \
	StepLinuxMceCD.h \
	StepDvdCss.c \
	StepDvdCss.h \
	StepNvidiaDrivers.c \
	StepNvidiaDrivers.h \
	StepNvidiaDriversEnd.c \
	StepNvidiaDriversEnd.h \
	RunShellCommand.c \
	RunShellCommand.h \
	StepCreateDiskedDevice.h \
	StepCreateDiskedDevice.c \
	StepConfFiles.h \
	StepConfFiles.c \
	StepCheckSystem.c \
	StepCheckSystem.h \
	StepNotification.h \
	StepNotification.c \
	StepUbuntuLiveCD.c \
	StepUbuntuLiveCD.h


mce_installer_LDFLAGS = 

mce_installer_LDADD = $(PACKAGE_LIBS)

EXTRA_DIST = $(glade_DATA)
