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

# Makefile for Hai.exe
# Microsoft 32-bit Visual C++ 4.0 or later 

# To use, do "nmake /f hai.mak"

# Define serial support once it is available for MSVC WIN32:
CFLAGS= -W3 -DWIN32 -DREADLINE_SUPPORT #-DHAISERIAL_SUPPORT 
CC=cl
LD=link
LDFLAGS=
O=.obj
H=.h

# variables
OBJ1 = help$(O) hai_serial$(O) hai_net$(O) omni_protocol$(O) aes$(O) readline$(O)
HEADERS = hai$(H) hai_net$(H) hai_serial$(H) omni_protocol$(H) help$(H) readline$(H)

all:  hai.exe 

hai.obj: hai.c $(HEADERS) $(OBJ1)
  $(CC) -c $(CFLAGS) $*.c

aes.obj: aes.c $(HEADERS)
  $(CC) -c $(CFLAGS) $*.c

hai_net.obj: hai_net.c $(HEADERS)
  $(CC) -c $(CFLAGS) $*.c

hai_serial.obj: hai_serial.c $(HEADERS)
  $(CC) -c $(CFLAGS) $*.c

help.obj: help.c $(HEADERS)
  $(CC) -c $(CFLAGS) $*.c

omni_protocol.obj: omni_protocol.c $(HEADERS) 
  $(CC) -c $(CFLAGS) $*.c

readline.obj: readline.c $(HEADERS) 
  $(CC) -c $(CFLAGS) $*.c

hai.exe: hai.obj 
  $(LD) $(LDFLAGS) hai$(O) $(OBJ1) ws2_32.lib /OUT:hai.exe /SUBSYSTEM:CONSOLE

build : clean all

clean :
	del $(OBJ1) hai.obj hai.exe

