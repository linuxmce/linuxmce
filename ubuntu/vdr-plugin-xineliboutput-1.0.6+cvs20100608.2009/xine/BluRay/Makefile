#
# deps
#

BLURAY_INC_DIR ?= /usr/src/libbluray/src/

#
# select plugins
#

ENABLE_DEMUX_PLUGIN ?= yes

ifeq ($(shell grep BLURAY $(BLURAY_INC_DIR)/bluray.h > /dev/null && echo yes), yes)
    ENABLE_INPUT_PLUGIN ?= yes
else
    ENABLE_INPUT_PLUGIN ?= no
    $(warning libbluray not found)
endif

ifeq ($(shell pkg-config libxine --atleast-version=1.1.18.2 || echo "yes"), yes)
    ENABLE_SPU_PLUGIN ?= yes
else
    ENABLE_SPU_PLUGIN ?= no
    $(warning SPU plugin is included in xine-lib 1.1.17+)
endif

ifeq ($(shell pkg-config libxine --atleast-version=1.1.17.1 || echo "yes"), yes)
    ENABLE_PCM_PLUGIN ?= yes
else
    ENABLE_PCM_PLUGIN ?= no
    $(warning PCM plugin is included in xine-lib 1.1.17.1+)
endif

#
# compiler options
#

CFLAGS      += -O2 -fPIC
CFLAGS      += -Wall
CFLAGS      += -D_REENTRANT -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS      += $(shell pkg-config libxine --cflags)
CFLAGS      += $(CFLAGS_BD)
INSTALL      = install

ifeq ($(ENABLE_INPUT_PLUGIN), yes)
CFLAGS_BD   ?= -I$(BLURAY_INC_DIR)
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
# targets
#

DESTDIR           ?= /
XINEPLUGINDIR      = $(shell pkg-config libxine --variable=plugindir)

ifeq ($(ENABLE_DEMUX_PLUGIN), yes)
    XINEDMXPLUGIN  = xineplug_dmx_mpeg_ts_hdmv.so
else
    $(warning Not building HDMV MPEG-TS demuxer plugin)
endif

ifeq ($(ENABLE_SPU_PLUGIN), yes)
    XINESPUPLUGIN = xineplug_decode_spuhdmv.so
else
    $(warning Not building HDMV SPU plugin)
endif

ifeq ($(ENABLE_INPUT_PLUGIN), yes)
    XINEINPUTPLUGIN= xineplug_inp_bluray.so
else
    $(warning Not building BluRay input plugin)
endif

ifeq ($(ENABLE_PCM_PLUGIN), yes)
    XINELPCMPLUGIN = xineplug_decode_lpcm_hdmv.so
else
    $(warning Not building BluRay PCM plugin)
endif

#
# object files
#

OBJS_XINEDMXPLUGIN   = demux_ts.o
OBJS_XINESPUPLUGIN   = decode_spuhdmv.o
OBJS_XINEINPUTPLUGIN = input_bluray.o
OBJS_XINELPCMPLUGIN  = xine_lpcm_decoder.o

#
# rules
#

all: $(XINEDMXPLUGIN) $(XINESPUPLUGIN) $(XINEINPUTPLUGIN) $(XINELPCMPLUGIN)

ifeq ($(ENABLE_DEMUX_PLUGIN), yes)
$(XINEDMXPLUGIN): $(OBJS_XINEDMXPLUGIN)
	$(CC) $(LDFLAGS) $(LIBS_XINE) -o $@ $(OBJS_XINEDMXPLUGIN)
endif

ifeq ($(ENABLE_SPU_PLUGIN), yes)
$(XINESPUPLUGIN): $(OBJS_XINESPUPLUGIN)
	$(CC) $(LDFLAGS) $(LIBS_XINE) -o $@ $(OBJS_XINESPUPLUGIN)
endif

ifeq ($(ENABLE_INPUT_PLUGIN), yes)
$(XINEINPUTPLUGIN): $(OBJS_XINEINPUTPLUGIN)
	$(CC) $(LDFLAGS) $(LIBS_XINE) $(LIBS_BD) -o $@ $(OBJS_XINEINPUTPLUGIN)
endif

ifeq ($(ENABLE_PCM_PLUGIN), yes)
$(XINELPCMPLUGIN): $(OBJS_XINELPCMPLUGIN)
	$(CC) $(LDFLAGS) $(LIBS_XINE) -o $@ $(OBJS_XINELPCMPLUGIN)
endif

#
# targets
#

clean:
	@rm -rf *.o *.so *~

install: all uninstall
ifeq ($(ENABLE_DEMUX_PLUGIN), yes)
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEDMXPLUGIN)
	@$(INSTALL) -m 0644 $(XINEDMXPLUGIN) $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEDMXPLUGIN)
endif
ifeq ($(ENABLE_SPU_PLUGIN), yes)
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINESPUPLUGIN)
	@$(INSTALL) -m 0644 $(XINESPUPLUGIN) $(DESTDIR)/$(XINEPLUGINDIR)/$(XINESPUPLUGIN)
endif
ifeq ($(ENABLE_INPUT_PLUGIN), yes)
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)
	@$(INSTALL) -m 0644 $(XINEINPUTPLUGIN) $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)
endif
ifeq ($(ENABLE_PCM_PLUGIN), yes)
	@echo Installing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINELPCMPLUGIN)
	@$(INSTALL) -m 0644 $(XINELPCMPLUGIN) $(DESTDIR)/$(XINEPLUGINDIR)/$(XINELPCMPLUGIN)
endif

uninstall:
ifeq ($(ENABLE_DEMUX_PLUGIN), yes)
	@echo Removing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEDMXPLUGIN)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEDMXPLUGIN)
endif
ifeq ($(ENABLE_SPU_PLUGIN), yes)
	@echo Removing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINESPUPLUGIN)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/$(XINESPUPLUGIN)
endif
ifeq ($(ENABLE_INPUT_PLUGIN), yes)
	@echo Removing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/$(XINEINPUTPLUGIN)
endif
ifeq ($(ENABLE_PCM_PLUGIN), yes)
	@echo Removing $(DESTDIR)/$(XINEPLUGINDIR)/$(XINELPCMPLUGIN)
	@-rm -rf $(DESTDIR)/$(XINEPLUGINDIR)/$(XINELPCMPLUGIN)
endif
