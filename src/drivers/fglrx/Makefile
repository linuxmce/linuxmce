GCC_VER_MAJ      = $(shell $(CC) --version | head -n 1 | sed -e 's/[^0-9. ]//g;s/^ *//;s/^\(.\)\..*$$/\1/')

obj-m           += fglrx.o
fglrx-libs      += libfglrx_ip.a.GCC$(GCC_VER_MAJ)
fglrx-c-objs    += firegl_public.o
fglrx-objs      += $(fglrx-c-objs) $(fglrx-libs)
fglrx-hdrs      += firegl_public.h
drm-hdrs        += drm.h drmP.h drm_os_linux.h drm_proc.h drm_compat.h

ifeq ($(PAGE_ATTR_FIX),)
PAGE_ATTR_FIX   =1
endif

EXTRA_CFLAGS    += \
                -I$(PWD) \
                -DFGL \
                -DFGL_LINUX \
                -DFGL_GART_RESERVED_SLOT \
                -DFGL_LINUX253P1_VMA_API \
                -DPAGE_ATTR_FIX=$(PAGE_ATTR_FIX) \

ifeq ($(KERNELRELEASE),)
# on first call from remote location we get into this path
# whilst on second call all is managed by the embedding kernel makefile

KVER            ?= $(shell uname -r)
KDIR            ?= /lib/modules/$(KVER)/build
PWD             = $(shell pwd)

fglrx-cfiles    = ${fglrx-c-objs:.o=.c}

# default:: kmod_build

kmod_build:: $(fglrx-libs) $(fglrx-cfiles) $(fglrx-hdrs) $(drm-hdrs)
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

%.c:
	@ln -s ../$@

%.h:
	@ln -s ../$@

%.GCC$(GCC_VER_MAJ):
	@ln -s ../$@

# distclean:: clean

clean::
	rm -rf $(PWD)/.tmp_versions
	rm -f *.o *.ko

endif

# ### EOF ###

install: 
	mkdir -p /lib/modules/
	cp *.ko $(DESTPATH)/lib/modules/
