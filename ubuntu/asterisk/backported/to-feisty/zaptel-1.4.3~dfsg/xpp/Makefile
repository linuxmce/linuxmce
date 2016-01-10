ZAPTEL_DIR	= $(SUBDIRS)

EXTRA_CFLAGS	=	$(XPP_LOCAL_CFLAGS)	\
			-g3 -I$(ZAPTEL_DIR)	\
			-DDEBUG			\
			-DPOLL_DIGITAL_INPUTS	\
			-DWITH_ECHO_SUPPRESSION	\
			-DPROTOCOL_DEBUG

ifneq	(,$(shell grep -w echo_can_state_t $(ZAPTEL_DIR)/zaptel.h))
EXTRA_CFLAGS	+=	-DZAPTEL_EC_TYPEDEF
endif

obj-m		+= xpp.o xpd_fxs.o xpd_fxo.o

HAS_BRISTUFF			:= $(shell cpp $(CPPFLAGS) -dM $(ZAPTEL_DIR)/zconfig.h | sed -n 's/^.*CONFIG_ZAPATA_BRI_DCHANS/y/p')

# Build only supported modules
ifneq	(,$(filter y m,$(CONFIG_USB)))
obj-m		+= xpp_usb.o
endif
ifneq	(,$(HAS_BRISTUFF))
obj-m		+= xpd_bri.o
endif

xpp-y		+= xbus-core.o xpp_zap.o xproto.o card_global.o
xpd_fxs-y	+= card_fxs.o
xpd_fxo-y	+= card_fxo.o
xpd_bri-y	+= card_bri.o

ifeq	(y,$(PARPORT_DEBUG))
EXTRA_CFLAGS	+= -DDEBUG_SYNC_PARPORT
obj-m		+= parport_debug.o
endif

ctags:
	ctags *.[ch]

# Handle versioning
XPP_VERSION_STR	?= $(shell if [ -r $(obj)/.version ]; then echo "\"`cat $(obj)/.version`\""; else echo '"Unknown"'; fi)
clean-files	:= xpp_version.h

$(obj)/card_fxs.o $(obj)/card_fxo.o $(obj)/card_bri.o $(obj)/xpp_usb.o $(obj)/xpp.o: $(obj)/xpp_version.h

$(obj)/xpp_version.h: FORCE
	$(Q)echo "Compile xpp (version $(XPP_VERSION_STR))"
	$(Q)echo '#define	XPP_VERSION	$(XPP_VERSION_STR)' > $@.tmp
	$(Q)if cmp -s $@.tmp $@ ; then echo; else \
		mv $@.tmp $@ ; \
	fi
	$(Q)rm -f $@.tmp

.PHONY: FORCE
FORCE:
