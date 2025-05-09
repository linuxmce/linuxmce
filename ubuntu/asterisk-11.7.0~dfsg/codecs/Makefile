#
# Asterisk -- A telephony toolkit for Linux.
# 
# Makefile for codec modules
#
# Copyright (C) 1999-2006, Digium, Inc.
#
# Mark Spencer <markster@digium.com>
#
# This program is free software, distributed under the terms of
# the GNU General Public License
#

-include $(ASTTOPDIR)/menuselect.makeopts $(ASTTOPDIR)/menuselect.makedeps

MODULE_PREFIX=codec
MENUSELECT_CATEGORY=CODECS
MENUSELECT_DESCRIPTION=Codec Translators

SUB_GSM := gsm

SUB_ILBC := ilbc
LIBILBC := $(SUB_ILBC)/libilbc.a

SUB_LPC10 := lpc10
LIBLPC10 := $(SUB_LPC10)/liblpc10.a

SUB_DIRS := \
	$(SUB_GSM) \
	$(SUB_LPC10) \

all: _all

include $(ASTTOPDIR)/Makefile.moddir_rules


ifneq ($(GSM_INTERNAL),no)
GSM_INCLUDE := -I$(SUB_GSM)/inc

$(if $(filter codec_gsm,$(EMBEDDED_MODS)),modules.link,codec_gsm.so): $(SUB_GSM)/lib/libgsm.a
endif

# Don't run the implicit rules for this target.
$(SUB_GSM)/lib/libgsm.a: $(SUB_GSM) ;

$(SUB_GSM):
	@mkdir -p $(SUB_GSM)/lib
	@$(MAKE) -C $(SUB_GSM) lib/libgsm.a


clean::
	for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	rm -f g722/*.[oai]
	rm -f speex/*.[oai]

.PHONY: $(SUB_DIRS)


$(if $(filter codec_lpc10,$(EMBEDDED_MODS)),modules.link,codec_lpc10.so): $(LIBLPC10)

# Don't run the implicit rules for this target.
$(LIBLPC10): $(SUB_LPC10) ;

$(SUB_LPC10):
	@$(MAKE) -C $(SUB_LPC10) all


ifneq ($(ILBC_INTERNAL),no)
$(if $(filter codec_ilbc,$(EMBEDDED_MODS)),modules.link,codec_ilbc.so): $(LIBILBC)
else
ILBC_INCLUDE += -DILBC_WEBRTC
endif

# Don't run the implicit rules for this target.
$(LIBILBC): $(SUB_ILBC) ;

$(SUB_ILBC):
	@$(MAKE) -C $(SUB_ILBC) all _ASTCFLAGS="$(filter-out -Wmissing-prototypes -Wmissing-declarations -Wshadow,$(_ASTCFLAGS)) $(AST_NO_STRICT_OVERFLOW)"


$(if $(filter codec_g722,$(EMBEDDED_MODS)),modules.link,codec_g722.so): g722/g722_encode.o g722/g722_decode.o

g722/g722_encode.o g722/g722_decode.o: _ASTCFLAGS+=$(call MOD_ASTCFLAGS,codec_g722)


ifeq ($(BUILD_CPU),x86_64)
SPEEX_RESAMPLE_CFLAGS:=-fPIC
else
SPEEX_RESAMPLE_CFLAGS:=
endif

$(if $(filter codec_resample,$(EMBEDDED_MODS)),modules.link,codec_resample.so): speex/resample.o

speex/resample.o: _ASTCFLAGS+=$(call MOD_ASTCFLAGS,codec_resample) $(SPEEX_RESAMPLE_CFLAGS)
