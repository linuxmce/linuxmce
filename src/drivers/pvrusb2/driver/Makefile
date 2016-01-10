
# Mike Isely <isely@pobox.com>

# This is the module build file for pvrusb2.  It requires the kbuild
# system from any 2.6.x kernel.  It requires the kbuild system from
# any 2.6.x kernel (but it's only been tried against kernels 2.6.10
# and later).  This WILL NOT BUILD for 2.4.x kernels.  Don't even
# bother trying.  Even if you were to fix this build for 2.4.x, you
# would still have to port the driver as well.  Everything here
# assumes 2.6.x.

# To build, you can just run this Makefile.  There are several
# variables you may want to override however:

# KDIR - Path to kernel source tree
# KREL - Version of kernel, i.e. 'uname -r' output
# INSTALL_MOD_DIR - where within the module tree to put the driver

# If you do not override anything, then KREL is set to the result of
# 'uname -r', KDIR is set to '/lib/modules/$(KREL)/build', and
# INSTALL_MOD_DIR is set to 'pvrusb2'.  If you choose to override
# KDIR, then you do _NOT_ need to worry about KREL, as KREL is only
# used here when calculating KDIR.  If the default path for KDIR is
# only wrong in terms of version element, then you can just override
# KREL with the corrected value.

# Sensible build targets include 'modules' (same as no target),
# 'install', and 'clean'

ifeq ($(KERNELRELEASE),)

  # Override any of these if you'd like
  ifeq ($(strip $(KREL)),)
    KREL := $(shell uname -r)
  endif
  ifeq ($(strip $(KDIR)),)
    KDIR := /lib/modules/$(KREL)/build
  endif
  INSTALL_MOD_DIR := pvrusb2

  .PHONY: all default install clean modules
  default: all
  all: modules

  M:=$(shell pwd)

  # Main option to select the entire driver
  CONFIG_VARS:= CONFIG_VIDEO_PVRUSB2=m

  # Select support for newer hardware
  CONFIG_VARS+= CONFIG_VIDEO_PVRUSB2_24XXX=y

  # Build sysfs module
  CONFIG_VARS+= CONFIG_VIDEO_PVRUSB2_SYSFS=y

  # Build dvb module
  CONFIG_VARS+= CONFIG_VIDEO_PVRUSB2_DVB=y

  # Build debug interface (you need this for manual firmware extraction)
  CONFIG_VARS+= CONFIG_VIDEO_PVRUSB2_DEBUGIFC=y

  # Enable extra stuff for debugging
  CONFIG_VARS+= CONFIG_VIDEO_ADV_DEBUG=y

  # Tiny hack to enable sparse tool checking
  ifeq ($(SPARSE),y)
    CONFIG_VARS += C=1
  endif

  modules modules_install clean:
	$(MAKE) INSTALL_MOD_DIR=$(INSTALL_MOD_DIR) -C $(KDIR) M=$(M) $(CONFIG_VARS) $@

  install:
	$(MAKE) INSTALL_MOD_DIR=$(INSTALL_MOD_DIR) -C $(KDIR) M=$(M) $(CONFIG_VARS) modules_install

else

  # Backwards compatibility in case kbuild can't find Kbuild on its own.
  include Kbuild

endif

