#
# Asterisk -- A telephony toolkit for Linux.
# 
# Makefile for channel drivers
#
# Copyright (C) 1999-2008, Digium, Inc.
#
# This program is free software, distributed under the terms of
# the GNU General Public License
#

-include $(ASTTOPDIR)/menuselect.makeopts $(ASTTOPDIR)/menuselect.makedeps

MODULE_PREFIX=chan
MENUSELECT_CATEGORY=CHANNELS
MENUSELECT_DESCRIPTION=Channel Drivers

H323OBJS=ooCmdChannel.o ooLogChan.o ooUtils.o ooGkClient.o context.o \
	ooDateTime.o decode.o dlist.o encode.o errmgmt.o \
	memheap.o ootrace.o oochannels.o ooh245.o ooports.o \
	ooq931.o ooCapability.o ooSocket.o perutil.o eventHandler.o \
	ooCalls.o ooStackCmds.o ooh323.o ooh323ep.o printHandler.o \
	rtctype.o ooTimer.o h323/H235-SECURITY-MESSAGESDec.o \
	h323/H235-SECURITY-MESSAGESEnc.o h323/H323-MESSAGES.o h323/H323-MESSAGESDec.o \
	h323/H323-MESSAGESEnc.o h323/MULTIMEDIA-SYSTEM-CONTROL.o \
	h323/MULTIMEDIA-SYSTEM-CONTROLDec.o h323/MULTIMEDIA-SYSTEM-CONTROLEnc.o

H323CFLAGS=-Iooh323c/src -Iooh323c/src/h323

all: _all

include $(ASTTOPDIR)/Makefile.moddir_rules

clean::
	rm -f $(addprefix ooh323c/src/,$(H323OBJS))

dist-clean::

chan_ooh323.o: ASTCFLAGS+=$(H323CFLAGS)

$(if $(filter chan_ooh323,$(EMBEDDED_MODS)),modules.link,chan_ooh323.so): ASTCFLAGS+=$(H323CFLAGS)
$(if $(filter chan_ooh323,$(EMBEDDED_MODS)),modules.link,chan_ooh323.so): $(addprefix ooh323c/src/,$(H323OBJS)) chan_ooh323.o ooh323cDriver.o
