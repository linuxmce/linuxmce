#
# Asterisk -- A telephony toolkit for Linux.
# 
# Makefile for Asterisk-addons
#
# Copyright (C) 1999-2008, Digium, Inc.
#
# This program is free software, distributed under the terms of
# the GNU General Public License
#

export ASTERISK_INCLUDE
export ASTCFLAGS
export ASTTOPDIR
export CC
export DESTDIR
export INSTALL
export MODULES_DIR
export NOISY_BUILD
export SOLINK

#NOISY_BUILD=yes

empty:=
space:=$(empty) $(empty)
ASTTOPDIR:=$(subst $(space),\$(space),$(CURDIR))

# Overwite config files on "make samples"
OVERWRITE:=y

# If the file .asteriskaddons.makeopts is present in your home directory, you can
# include all of your favorite menuselect options so that every time you download
# a new version of Asterisk-addons, you don't have to run menuselect to set them.
# The file /etc/asteriskaddons.makeopts will also be included but can be overridden
# by the file in your home directory.

GLOBAL_MAKEOPTS=$(wildcard /etc/asteriskaddons.makeopts)
USER_MAKEOPTS=$(wildcard ~/.asteriskaddons.makeopts)

MOD_SUBDIR_CFLAGS=$(ASTERISK_INCLUDE)
OTHER_SUBDIR_CFLAGS=$(ASTERISK_INCLUDE)

#ifeq ($(strip $(foreach var,clean distclean dist-clean update,$(findstring $(var),$(MAKECMDGOALS)))),)
# ifneq ($(wildcard menuselect.makeopts),)
#  include menuselect.makeopts
#  include menuselect.makedeps
# endif
#endif

ifneq ($(wildcard makeopts),)
  include makeopts
endif

ifeq ($(AST_DEVMODE),yes)
  ASTCFLAGS+=-Werror -Wunused -Wundef $(AST_DECLARATION_AFTER_STATEMENT) -Wmissing-format-attribute -Wformat-security -Wformat=2
endif

ifeq ($(OSARCH),SunOS)
  ASTETCDIR=/var/etc/asterisk
  ASTLIBDIR=/opt/asterisk/lib
  ASTVARLIBDIR=/var/opt/asterisk
else
  ASTETCDIR=$(sysconfdir)/asterisk
  ASTLIBDIR=$(libdir)/asterisk
ifneq ($(findstring BSD,$(OSARCH)),)
  ASTVARLIBDIR=$(prefix)/share/asterisk
else
  ASTVARLIBDIR=$(localstatedir)/lib/asterisk
endif
endif
ifeq ($(ASTDATADIR),)
  ASTDATADIR:=$(ASTVARLIBDIR)
endif

MODULES_DIR=$(ASTLIBDIR)/modules

#MOD_SUBDIRS:=channels pbx apps codecs formats cdr funcs tests main res $(LOCAL_MOD_SUBDIRS)
MOD_SUBDIRS:=channels apps formats cdr res $(LOCAL_MOD_SUBDIRS)
OTHER_SUBDIRS:=
SUBDIRS:=$(OTHER_SUBDIRS) $(MOD_SUBDIRS)
SUBDIRS_INSTALL:=$(SUBDIRS:%=%-install)
SUBDIRS_CLEAN:=$(SUBDIRS:%=%-clean)
SUBDIRS_DIST_CLEAN:=$(SUBDIRS:%=%-dist-clean)
SUBDIRS_UNINSTALL:=$(SUBDIRS:%=%-uninstall)
MOD_SUBDIRS_EMBED_LDSCRIPT:=$(MOD_SUBDIRS:%=%-embed-ldscript)
MOD_SUBDIRS_EMBED_LDFLAGS:=$(MOD_SUBDIRS:%=%-embed-ldflags)
MOD_SUBDIRS_EMBED_LIBS:=$(MOD_SUBDIRS:%=%-embed-libs)
MOD_SUBDIRS_MENUSELECT_TREE:=$(MOD_SUBDIRS:%=%-menuselect-tree)

ifeq ($(OSARCH),Darwin)
SOLINK=-dynamic -bundle -undefined suppress -force_flat_namespace
else
SOLINK=-shared -Xlinker -x
endif
ifeq ($(OSARCH),SunOS)
SOLINK=-shared -fpic -L$(CROSS_COMPILE_TARGET)/usr/local/ssl/lib
endif

# comment to print directories during submakes
#PRINT_DIR=yes

SILENTMAKE:=$(MAKE) --quiet --no-print-directory
ifneq ($(PRINT_DIR)$(NOISY_BUILD),)
SUBMAKE:=$(MAKE) --quiet
else
SUBMAKE:=$(MAKE) --quiet --no-print-directory
endif

# $(MAKE) is printed in several places, and we want it to be a
# fixed size string. Define a variable whose name has also the
# same size, so we can easily align text.
ifeq ($(MAKE), gmake)
	mK="gmake"
else
	mK=" make"
endif

all: _all
	@echo " +----	 Asterisk-Addons Build Complete ----+"
	@echo " +                                           +"
	@echo " +   Addons has successfully been built .    +"
	@echo " +   If you would like to install it :       +"
	@echo " +                                           +"
	@echo " +               $(mK) install               +"  
	@echo " +-------------------------------------------+"  

_all: cleantest makeopts $(SUBDIRS) doc/addons-en_US.xml

makeopts: configure
	@echo "****"
	@echo "**** The configure script must be executed before running '$(MAKE)'." 
	@echo "****               Please run \"./configure\"."
	@echo "****"
	@exit 1

menuselect.makeopts: menuselect/menuselect menuselect-tree makeopts
	menuselect/menuselect --check-deps $@ $(GLOBAL_MAKEOPTS) $(USER_MAKEOPTS)

$(MOD_SUBDIRS_EMBED_LDSCRIPT):
	@echo "EMBED_LDSCRIPTS+="`$(SILENTMAKE) -C $(@:-embed-ldscript=) SUBDIR=$(@:-embed-ldscript=) __embed_ldscript` >> makeopts.embed_rules

$(MOD_SUBDIRS_EMBED_LDFLAGS):
	@echo "EMBED_LDFLAGS+="`$(SILENTMAKE) -C $(@:-embed-ldflags=) SUBDIR=$(@:-embed-ldflags=) __embed_ldflags` >> makeopts.embed_rules

$(MOD_SUBDIRS_EMBED_LIBS):
	@echo "EMBED_LIBS+="`$(SILENTMAKE) -C $(@:-embed-libs=) SUBDIR=$(@:-embed-libs=) __embed_libs` >> makeopts.embed_rules

$(MOD_SUBDIRS_MENUSELECT_TREE):
	@$(SUBMAKE) -C $(@:-menuselect-tree=) SUBDIR=$(@:-menuselect-tree=) moduleinfo
	@$(SUBMAKE) -C $(@:-menuselect-tree=) SUBDIR=$(@:-menuselect-tree=) makeopts

makeopts.embed_rules: menuselect.makeopts
	@echo "Generating embedded module rules ..."
	@rm -f $@
	@$(MAKE) $(PRINT_DIR) $(MOD_SUBDIRS_EMBED_LDSCRIPT)
	@$(MAKE) $(PRINT_DIR) $(MOD_SUBDIRS_EMBED_LDFLAGS)
	@$(MAKE) $(PRINT_DIR) $(MOD_SUBDIRS_EMBED_LIBS)

$(SUBDIRS): makeopts.embed_rules

$(MOD_SUBDIRS):
	@ASTCFLAGS="$(MOD_SUBDIR_CFLAGS) $(ASTCFLAGS)" ASTLDFLAGS="$(ASTLDFLAGS)" $(MAKE) $(PRINT_DIR) --no-builtin-rules -C $@ SUBDIR=$@ all

$(OTHER_SUBDIRS):
	@ASTCFLAGS="$(OTHER_SUBDIR_CFLAGS) $(ASTCFLAGS)" ASTLDFLAGS="$(ASTLDFLAGS)" $(MAKE) $(PRINT_DIR) --no-builtin-rules -C $@ SUBDIR=$@ all

config.status: configure
	@./configure
	@echo "****"
	@echo "**** The configure script was just executed, so 'make' needs to be"
	@echo "**** restarted."
	@echo "****"
	@exit 1

install: _all $(SUBDIRS_INSTALL) install-xmldoc 
	@echo " +---- Asterisk-Addons Installation Complete ----+"
	@echo " +                                               +"
	@echo " +    Addons has successfully been installed.    +"
	@echo " +    If you would like to install the sample    +"
	@echo " +    configuration files (overwriting any       +"
	@echo " +    existing config files), run:               +"
	@echo " +                                               +"
	@echo " +               $(MAKE) samples                    +"
	@echo " +-----------------------------------------------+"

install-xmldoc: doc/addons-en_US.xml
	@echo "Installing XML documentation"
	@$(INSTALL) -m 644 doc/addons-*.xml $(ASTDATADIR)/documentation

cleantest:

$(SUBDIRS_INSTALL):
	@DESTDIR="$(DESTDIR)" ASTSBINDIR="$(ASTSBINDIR)" $(MAKE) --quiet $(PRINT_DIR) -C $(@:-install=) install

$(SUBDIRS_UNINSTALL):
	@$(MAKE) $(PRINT_DIR) -C $(@:-uninstall=) uninstall

$(SUBDIRS_CLEAN):
	@$(MAKE) $(PRINT_DIR) -C $(@:-clean=) clean

$(SUBDIRS_DIST_CLEAN):
	@$(MAKE) $(PRINT_DIR) -C $(@:-dist-clean=) dist-clean

clean: $(SUBDIRS_CLEAN)
	@$(MAKE) -C menuselect clean

dist-clean: distclean

distclean: $(SUBDIRS_DIST_CLEAN) clean
	@$(MAKE) -C menuselect dist-clean
	rm -f menuselect.makeopts makeopts menuselect-tree menuselect.makedeps
	rm -f makeopts.embed_rules
	rm -f config.log config.status
	rm -rf autom4te.cache
	rm -f build_tools/menuselect-deps

samples: 
	mkdir -p $(DESTDIR)$(ASTETCDIR)
	for x in configs/*.sample; do \
		if [ -f $(DESTDIR)$(ASTETCDIR)/`basename $$x .sample` ]; then \
			if [ "$(OVERWRITE)" = "y" ]; then \
				if cmp -s $(DESTDIR)$(ASTETCDIR)/`basename $$x .sample` $$x ; then \
					echo "Config file $$x is unchanged"; \
					continue; \
				fi ; \
				mv -f $(DESTDIR)$(ASTETCDIR)/`basename $$x .sample` $(DESTDIR)$(ASTETCDIR)/`basename $$x .sample`.old ; \
			else \
				echo "Skipping config file $$x"; \
				continue; \
			fi ;\
		fi ; \
		$(INSTALL) -m 644 $$x $(DESTDIR)$(ASTETCDIR)/`basename $$x .sample` ;\
	done

doc/addons-en_US.xml: $(foreach dir,$(MOD_SUBDIRS),$(wildcard $(dir)/*.c) $(wildcard $(dir)/*.cc))
	@echo -n "Building Documentation For: "
	@echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" > $@
	@echo "<!DOCTYPE docs SYSTEM \"appdocsxml.dtd\">" >> $@
	@echo "<docs>" >> $@
	@for x in $(MOD_SUBDIRS); do \
		echo -n "$$x " ; \
		for i in $$x/*.c; do \
			$(AWK) -f build_tools/get_documentation $$i >> $@ ; \
		done ; \
	done
	@echo
	@echo "</docs>" >> $@

validate-docs: install-xmldoc 
ifeq ($(XMLSTARLET),:)
	@echo "---------------------------------------------------------------"
	@echo "--- Please install xmlstarlet to validate the documentation ---"
	@echo "---------------------------------------------------------------"
else
	@$(XMLSTARLET) val -E $(ASTDATADIR)/documentation/addons-en_US.xml 
endif

update:
	@if [ -d .svn ]; then \
		echo "Updating from Subversion..." ; \
		fromrev="`svn info | $(AWK) '/Revision: / {print $$2}'`"; \
		svn update | tee update.out; \
		torev="`svn info | $(AWK) '/Revision: / {print $$2}'`"; \
		echo "`date`  Updated from revision $${fromrev} to $${torev}." >> update.log; \
		rm -f .version; \
		if [ `grep -c ^C update.out` -gt 0 ]; then \
			echo ; echo "The following files have conflicts:" ; \
			grep ^C update.out | cut -b4- ; \
		fi ; \
		rm -f update.out; \
	else \
		echo "Not under version control"; \
	fi

menuconfig: menuselect

cmenuconfig: cmenuselect

gmenuconfig: gmenuselect

nmenuconfig: nmenuselect

menuselect: menuselect/cmenuselect menuselect/nmenuselect menuselect/gmenuselect
	@if [ -x menuselect/nmenuselect ]; then \
		$(MAKE) nmenuselect; \
	elif [ -x menuselect/cmenuselect ]; then \
		$(MAKE) cmenuselect; \
	elif [ -x menuselect/gmenuselect ]; then \
		$(MAKE) gmenuselect; \
	else \
		echo "No menuselect user interface found. Install ncurses,"; \
		echo "newt or GTK libraries to build one and re-rerun"; \
		echo "'make menuselect'."; \
	fi

cmenuselect: menuselect/cmenuselect menuselect-tree
	-@menuselect/cmenuselect menuselect.makeopts $(GLOBAL_MAKEOPTS) $(USER_MAKEOPTS) && echo "menuselect changes saved!" || echo "menuselect changes NOT saved!"

gmenuselect: menuselect/gmenuselect menuselect-tree
	-@menuselect/gmenuselect menuselect.makeopts $(GLOBAL_MAKEOPTS) $(USER_MAKEOPTS) && echo "menuselect changes saved!" || echo "menuselect changes NOT saved!"

nmenuselect: menuselect/nmenuselect menuselect-tree
	-@menuselect/nmenuselect menuselect.makeopts $(GLOBAL_MAKEOPTS) $(USER_MAKEOPTS) && echo "menuselect changes saved!" || echo "menuselect changes NOT saved!"

# options for make in menuselect/
MAKE_MENUSELECT=CC="$(HOST_CC)" CXX="$(CXX)" LD="" AR="" RANLIB="" CFLAGS="" $(MAKE) -C menuselect CONFIGURE_SILENT="--silent"

menuselect/menuselect: menuselect/makeopts
	$(MAKE_MENUSELECT) menuselect

menuselect/cmenuselect: menuselect/makeopts
	$(MAKE_MENUSELECT) cmenuselect

menuselect/gmenuselect: menuselect/makeopts
	$(MAKE_MENUSELECT) gmenuselect

menuselect/nmenuselect: menuselect/makeopts
	$(MAKE_MENUSELECT) nmenuselect

menuselect/makeopts: makeopts
	$(MAKE_MENUSELECT) makeopts

menuselect-tree: $(foreach dir,$(filter-out main,$(MOD_SUBDIRS)),$(wildcard $(dir)/*.c) $(wildcard $(dir)/*.cc)) configure
	@echo "Generating input for menuselect ..."
	@echo "<?xml version=\"1.0\"?>" > $@
	@echo >> $@
	@echo "<menu name=\"Asterisk-addons Module Selection\">" >> $@
	@for dir in $(sort $(filter-out main,$(MOD_SUBDIRS))); do $(SUBMAKE) -C $${dir} SUBDIR=$${dir} moduleinfo >> $@; done
	@for dir in $(sort $(filter-out main,$(MOD_SUBDIRS))); do $(SUBMAKE) -C $${dir} SUBDIR=$${dir} makeopts >> $@; done
	@echo "</menu>" >> $@

.PHONY: menuselect clean dist-clean distclean all cleantest install-xmldoc uninstall _uninstall uninstall-all dont-optimize $(SUBDIRS_INSTALL) $(SUBDIRS_DIST_CLEAN) $(SUBDIRS_CLEAN) $(SUBDIRS_UNINSTALL) $(SUBDIRS) $(MOD_SUBDIRS_EMBED_LDSCRIPT) $(MOD_SUBDIRS_EMBED_LDFLAGS) $(MOD_SUBDIRS_EMBED_LIBS)
