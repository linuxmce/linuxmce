#
# Asterisk -- A telephony toolkit for Linux.
# 
# Makefile for file format modules
#
# Copyright (C) 1999-2008, Digium, Inc.
#
# This program is free software, distributed under the terms of
# the GNU General Public License
#

-include $(ASTTOPDIR)/menuselect.makeopts $(ASTTOPDIR)/menuselect.makedeps

MODULE_PREFIX=format
MENUSELECT_CATEGORY=FORMATS
MENUSELECT_DESCRIPTION=Format Interpreters

all: _all

include $(ASTTOPDIR)/Makefile.moddir_rules

clean::
	$(MAKE) -C mp3 clean

$(if $(filter format_mp3,$(EMBEDDED_MODS)),modules.link,format_mp3.so): mp3/common.o mp3/dct64_i386.o mp3/decode_ntom.o mp3/layer3.o mp3/tabinit.o mp3/interface.o
