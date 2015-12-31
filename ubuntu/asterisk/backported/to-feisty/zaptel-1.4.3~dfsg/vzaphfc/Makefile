#
#  Copyright (C) 2006 headissue GmbH; Jens Wilke
#
#  This program is free software and may be modified and
#  distributed under the terms of the GNU Public License.
#

KSOURCE ?= /lib/modules/`uname -r`/build

ifndef src
	src=$(shell pwd)
endif

ZAPTEL_SOURCE = $(src)/../../bristuff12/zaptel/

EXTRA_CFLAGS += -I $(ZAPTEL_SOURCE)

obj-m	:= vzaphfc.o
vzaphfc-objs := vzaphfc_main.o fifo.o lapd.o

all:
	$(MAKE) -C $(KSOURCE) SUBDIRS=`pwd` modules

clean:
	$(MAKE) -C $(KSOURCE) SUBDIRS=`pwd` clean

debug:
	$(MAKE) -C $(KSOURCE) modules SUBDIRS=`pwd` EXTRA_CFLAGS="$(EXTRA_CFLAGS) -DDEBUG" CONFIG_DEBUG_INFO=1

# 2.4 stuff ####
modules: $(obj-m)
	sync

vzaphfc.o: vzaphfc_main.c fifo.c vzaphfc.h fifo.h lapd.h
	$(CC) -c vzaphfc_main.c fifo.c $(CFLAGS) $(EXTRA_CFLAGS)

install:
	install -D -m 644 vzaphfc.ko $(INSTALL_PREFIX)/lib/modules/`uname -r`/misc/vzaphfc.ko
