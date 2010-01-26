#ENABLE_INPUT_PLUGIN=yes

#
# targets
#

XINEPLUGINDIR      = $(shell pkg-config libxine --variable=plugindir)

XINEDMXPLUGIN      = xineplug_dmx_mpeg_ts_hdmv.so
XINESPUPLUGIN      = xineplug_decode_spuhdmv.so
ifeq ($(ENABLE_INPUT_PLUGIN), yes)
XINEINPUTPLUGIN    = xineplug_inp_bluray.so
endif

#
# compiler options
#

CFLAGS      += -O2 -fPIC
CFLAGS      += $(shell pkg-config libxine --cflags)
CFLAGS      += $(CFLAGS_BD)
DESTDIR      = /
INSTALL      = install

ifeq ($(ENABLE_INPUT_PLUGIN), yes)
CFLAGS_BD   ?= -I/usr/src/libburay/libbluray/trunk/src/
endif

#
# linker options
#

LIBS_XINE    = $(shell pkg-config libxine --libs)
LDFLAGS     += -shared -fvisibility=hidden

ifeq ($(ENABLE_INPUT_PLUGIN), yes)
LIBS_BD     ?= -L/usr/local/lib -lbluray
endif

#
# object files
#

OBJS_XINEDMXPLUGIN   = demux_ts.o
OBJS_XINESPUPLUGIN   = decode_spuhdmv.o
OBJS_XINEINPUTPLUGIN = input_bluray.o

#
#
#

all: $(XINEDMXPLUGIN) $(XINESPUPLUGIN) $(XINEINPUTPLUGIN)

$(XINEDMXPLUGIN): $(OBJS_XINEDMXPLUGIN)
	$(CC) $(LDFLAGS) $(LIBS_XINE) -o $@ $(OBJS_XINEDMXPLUGIN)

$(XINESPUPLUGIN): $(OBJS_XINESPUPLUGIN)
	$(CC) $(LDFLAGS) $(LIBS_XINE) -o $@ $(OBJS_XINESPUPLUGIN)

ifeq ($(ENABLE_INPUT_PLUGIN), yes)
$(XINEINPUTPLUGIN): $(OBJS_XINEINPUTPLUGIN)
	$(CC) $(LDFLAGS) $(LIBS_XINE) $(LIBS_BD) -o $@ $(OBJS_XINEINPUTPLUGIN)
endif

clean:
	@rm -rf *.o *.so *~

install: all uninstall
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEDMXPLUGIN)
	@$(INSTALL) -m 0644 $(XINEDMXPLUGIN) $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEDMXPLUGIN)
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINESPUPLUGIN)
	@$(INSTALL) -m 0644 $(XINESPUPLUGIN) $(DESTDIR)/$(XINEPLUGINDIR)/$(XINESPUPLUGIN)
ifeq ($(ENABLE_INPUT_PLUGIN), yes)
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)
	@$(INSTALL) -m 0644 $(XINEINPUTPLUGIN) $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)
endif

uninstall:
	@echo Removing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEDMXPLUGIN)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEDMXPLUGIN)
	@echo Removing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINESPUPLUGIN)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/$(XINESPUPLUGIN)
ifeq ($(ENABLE_INPUT_PLUGIN), yes)
	@echo Removing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)
endif
