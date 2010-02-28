#-----------------------------------------------------------------------------#
# Copyright (C) 2004-2005  Chuck Cannon                                       #
#                                                                             #
# This program is free software; you can redistribute it and/or               #
# modify it under the terms of the GNU General Public License                 #
# as published by the Free Software Foundation; either version 2              #
# of the License, or (at your option) any later version.                      #
#                                                                             #
# This program is distributed in the hope that it will be useful,             #
# but WITHOUT ANY WARRANTY; without even the implied warranty of              #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               #
# GNU General Public License for more details.                                #
#                                                                             #
# You should have received a copy of the GNU General Public License           #
# along with this program; if not, write to the Free Software                 #
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. #
#-----------------------------------------------------------------------------#

#-----------------------------------------------------------------------------#
# If this makefile fails to detect Cygwin correctly, or you want to force
# the build process's behaviour, set it to "true" or "false" (w/o quotes).
#-----------------------------------------------------------------------------#
#cygwin := true
#cygwin := false
cygwin := autodetect

#-----------------------------------------------------------------------------#
# Build file definitions
#-----------------------------------------------------------------------------#

HAI = hai
HAIL = hail
HAIS = hais
SONAME = hai
VERSION = 1
MINOR = 0
RELEASE = 7

HAILIBSRCS = aes.c hai_net.c hai_serial.c omni_protocol.c
HAILIBDEFS = aes.h hai_comm.h hai_net.h hai_serial.h omni_protocol.h
HAILSRCS = hai.c help.c readline.c
HAILDEFS = hai.h help.h readline.h
HAISRCS = $(HAILSRCS) $(HAILIBSRCS)
HAIDEFS = $(HAILDEFS) $(HAILIBDEFS)

#-----------------------------------------------------------------------------#
# Install definitions
#-----------------------------------------------------------------------------#

INSTALLBIN = /usr/local/bin
INSTALLDLL = /usr/bin
INSTALLLIB = /usr/lib
INSTALLDEF = /usr/include

#-----------------------------------------------------------------------------#
# Detect cygwin
#-----------------------------------------------------------------------------#

ifeq ($(strip $(cygwin)),autodetect)
  ifneq ($(strip $(shell gcc -v 2>&1 |grep "cygwin")),)
    cygwin := true
  else
    cygwin := false
  endif
endif

#-----------------------------------------------------------------------------#
# Other definitions
#-----------------------------------------------------------------------------#

CC = gcc
CFLAGS = -O2 -Wall -DHAISERIAL_SUPPORT -DGNUREADLINE_SUPPORT

LINKER = gcc
LFLAGS = -lreadline -ltermcap

AR = ar
ARFLAGS = rcs


CLEANUP = $(HAIOBJS) $(HAISLIBOBJS) $(HAIEXE) $(HAILEXE) $(HAISEXE) \
		  $(HAILIB) $(HAISLIB) $(HAISONAME) core *.stackdump

HAIOBJS := $(HAISRCS:.c=.o)
HAILOBJS := $(HAILSRCS:.c=.o)
HAILIBOBJS := $(HAILIBSRCS:.c=.o)
HAISLIBOBJS := $(HAILIBSRCS:.c=.lo)

ifeq ($(strip $(cygwin)),true)
	HAIEXE = $(HAI).exe
	HAILEXE = $(HAIL).exe
	HAISEXE = $(HAIS).exe
	SLIBFLAGS =
	HAISONAME = libhai.dll.a
	HAISLIB = cyg$(SONAME).dll
	SAR = gcc -shared -Wl,--out-implib,
	SARFLAGS =
	DOX = "C:/Program Files/doxygen/bin/doxygen.exe"
else
	HAIEXE = $(HAI)
	HAILEXE = $(HAIL)
	HAISEXE = $(HAIS)
	SLIBFLAGS = -fPIC
	HAISONAME = lib$(SONAME).so.$(VERSION)
	HAISLIB = $(HAISONAME).$(MINOR).$(RELEASE)
	SAR = gcc -shared -Wl,-soname,
	SARFLAGS = -lc
	DOX = doxygen
endif

HAILIB = lib$(SONAME).a

#-----------------------------------------------------------------------------#
# Rules
#-----------------------------------------------------------------------------#

$(HAIEXE) : $(HAIOBJS)
	$(LINKER) -o $(HAIEXE) $(HAIOBJS) $(LFLAGS)

$(HAILEXE) : $(HAILOBJS)
	$(LINKER) -o $(HAILEXE) $(HAILOBJS) $(LFLAGS) -static -lhai

$(HAISEXE) : $(HAILOBJS)
	$(LINKER) -o $(HAISEXE) $(HAILOBJS) $(LFLAGS) -lhai

$(HAILIB) : $(HAILIBOBJS)
	$(AR) $(ARFLAGS) $(HAILIB) $(HAILIBOBJS)

$(HAISLIB) : $(HAISLIBOBJS)
	$(SAR)$(HAISONAME) -o $(HAISLIB) $(HAISLIBOBJS) $(SARFLAGS)

%.o: %.c $(HAIDEFS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.lo: %.c $(HAIDEFS)
	$(CC) $(SLIBFLAGS) -c -o $@ $< $(CFLAGS)

all: $(HAIEXE) $(HAILIB) $(HAISLIB)

clean :
	rm -f $(CLEANUP)

build : clean all

installall : install libinstall slibinstall

install : $(HAIEXE)
	cp $(HAIEXE) $(INSTALLBIN)/$(HAIEXE)
	chmod 755 $(INSTALLBIN)/$(HAIEXE)

libinstall : $(HAILIB)
	cp $(HAILIB) $(INSTALLLIB)/$(HAILIB)
	chmod 644 $(INSTALLLIB)/$(HAILIB)
	cp $(HAILIBDEFS) $(INSTALLDEF)

ifeq ($(strip $(cygwin)),true)
slibinstall : $(HAISLIB)
	cp $(HAISONAME) $(INSTALLLIB)/$(HAISONAME)
	chmod 644 $(INSTALLLIB)/$(HAISONAME)
	cp $(HAISLIB) $(INSTALLDLL)/$(HAISLIB)
	chmod 644 $(INSTALLDLL)/$(HAISLIB)
else
slibinstall : $(HAISLIB)
	cp $(HAISLIB) $(INSTALLLIB)/$(HAISLIB)
	chmod 644 $(INSTALLLIB)/$(HAISLIB)
	cp $(HAILIBDEFS) $(INSTALLDEF)
	/sbin/ldconfig -n $(INSTALLLIB)
	ln -f -s $(INSTALLLIB)/$(HAISLIB) $(INSTALLLIB)/lib$(SONAME).so
endif

dox :
	$(DOX) dox.cfg

