#
# $RCSfile: Makefile,v $  $Revision: 1.5 $  $Name:  $
# $Id: Makefile,v 1.5 2007/04/03 00:12:10 bpaauwe Exp $
# $Author: bpaauwe $
# $Date: 2007/04/03 00:12:10 $
# ----------------------------------------------------------------------------
#
#  Copyright (c) Bob Paauwe (2006)
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by the Free
#  Software Foundation; either version 2 of the License, or (at your option)
#  any later version.
#
#  This program is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
#  more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc.,
#  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# ----------------------------------------------------------------------------


SRC = ilib.c idrv.c ibios.c insteon_msg.c util.c ikd.c ihid.c iusb.c iser.c \
	  x10_cmd.c
INC = idrv.h insteon_msg.h
OBJ = insteon_msg.o idrv.o ibios.o util.o ilib.o ikd.o ihid.o iusb.o iser.o \
	  ilib_sm.o ikd.o iser.o x10_cmd.o

CFLAGS = -g -Wall -I/usr/local/include

ifeq ($(INCLUDE_LIBHID), 1)
CFLAGS += -DINCLUDE_LIBHID
endif

# To build with libhid support:
#CFLAGS = -g -Wall -DINCLUDE_LIBHID -I/usr/local/include

all: libilib.a

libilib.a: $(OBJ) $(INC) ikd.h ihid.h iusb.h iser.h
	$(AR) -rv libilib.a $(OBJ)

libidrv.a: $(OBJ) $(INC) ikd.o ikd.h
	$(AR) -rv libidrv.a $(OBJ) ikd.o

libihid.a: $(OBJ) $(INC) ihid.o ihid.h
	$(AR) -rv libihid.a $(OBJ) ihid.o

libiusb.a: $(OBJ) $(INC) iusb.o iusb.h
	$(AR) -rv libiusb.a $(OBJ) iusb.o

libiser.a: $(OBJ) $(INC) serb.o serb.h
	$(AR) -rv libiser.a $(OBJ) iser.o

.c.o :
	$(CC) $(CFLAGS) -c $<

clean:
	-rm *.a *.o
