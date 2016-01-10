#
# Asterisk -- A telephony toolkit for Linux.
# 
# Makefile for PBX applications
#
# Copyright (C) 1999-2006, Digium, Inc.
#
# This program is free software, distributed under the terms of
# the GNU General Public License
#

-include $(ASTTOPDIR)/menuselect.makeopts $(ASTTOPDIR)/menuselect.makedeps

MODULE_PREFIX=app
MENUSELECT_CATEGORY=APPS
MENUSELECT_DESCRIPTION=Applications

MODS_C_ADDITIONAL=app_voicemail_imapstorage app_voicemail_odbcstorage

MENUSELECT_OPTS_app_directory:=$(MENUSELECT_OPTS_app_voicemail)
ifneq ($(findstring ODBC_STORAGE,$(MENUSELECT_OPTS_app_voicemail)),)
  MENUSELECT_DEPENDS_app_voicemail+=$(MENUSELECT_DEPENDS_ODBC_STORAGE)
endif
ifneq ($(findstring IMAP_STORAGE,$(MENUSELECT_OPTS_app_voicemail)),)
  MENUSELECT_DEPENDS_app_voicemail+=$(MENUSELECT_DEPENDS_IMAP_STORAGE)
endif

all: _all

add_depends_cmd = sed -i -e '/^\/\*\*\* MODULEINFO/a\\t<depend>$(1)<\/depend>'
app_voicemail_imapstorage.c: app_voicemail.c
	cp $< $@
	$(call add_depends_cmd,imap_tk) $@
	$(call add_depends_cmd,openssl) $@
	cp app_voicemail.exports.in app_voicemail_imapstorage.exports.in
app_voicemail_odbcstorage.c: app_voicemail.c
	cp $< $@
	$(call add_depends_cmd,unixodbc) $@
	$(call add_depends_cmd,ltdl) $@
	cp app_voicemail.exports.in app_voicemail_odbcstorage.exports.in

dist-clean::
	rm -f app_voicemail_*.c
	rm -f app_voicemail_*.exports.in

app_voicemail_imapstorage.o: _ASTCFLAGS+=-DIMAP_STORAGE
app_voicemail_odbcstorage.o: _ASTCFLAGS+=-DODBC_STORAGE

include $(ASTTOPDIR)/Makefile.moddir_rules

clean::
	rm -f confbridge/*.o confbridge/*.i

$(if $(filter app_confbridge,$(EMBEDDED_MODS)),modules.link,app_confbridge.so): $(subst .c,.o,$(wildcard confbridge/*.c))
$(subst .c,.o,$(wildcard confbridge/*.c)): _ASTCFLAGS+=$(call MOD_ASTCFLAGS,app_confbridge)

ifneq ($(findstring $(OSARCH), mingw32 cygwin ),)
  LIBS+= -lres_features.so -lres_ael_share.so -lres_monitor.so -lres_speech.so
  LIBS+= -lres_smdi.so
endif

