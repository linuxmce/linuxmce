DIANEMO_SRC ?= ../..
LauncherPackages = glib-2.0 gtk+-2.0 hildon-1 dbus-1 gconf-2.0 conic libosso

build: .build-timestamp

.build-timestamp: orbiter-launcher Orbiter
	touch $@

.libs-timestamp: libs
	touch $@

libs:
	cd $(DIANEMO_SRC)/PlutoUtils; $(MAKE) clean; $(MAKE) SNR_CPPFLAGS="-DMAEMO_NOKIA770"
	cd $(DIANEMO_SRC)/SerializeClass; $(MAKE) clean; $(MAKE) SNR_CPPFLAGS="-DMAEMO_NOKIA770"
	cd $(DIANEMO_SRC)/SDL_Helpers; $(MAKE) clean; $(MAKE) SNR_CPPFLAGS="-DMAEMO_NOKIA770"
	cd $(DIANEMO_SRC)/DCE; $(MAKE) clean; $(MAKE) SNR_CPPFLAGS="-DMAEMO_NOKIA770"

Orbiter: .libs-timestamp
	cd $(DIANEMO_SRC)/Orbiter; $(MAKE) clean; $(MAKE) SNR_CPPFLAGS='-DMAEMO_NOKIA770 -DVERSION="\"'"$(COMPILE_VERSION)"'\"" -DCOMPILE_DATE="\"'"$(COMPILE_DATE)"'\""' LIBGPGME= bin

orbiter-launcher: orbiter.c
	$(CC) -DOS2008 $(shell pkg-config --cflags $(LauncherPackages)) -o $@ $< $(shell pkg-config --libs $(LauncherPackages))

install: .build-timestamp
	mkdir -p $(DESTDIR)/opt/linuxmce-orbiter/{bin,lib}
	cp $(DIANEMO_SRC)/bin/Orbiter orbiter-launcher{,.sh} $(DESTDIR)/opt/linuxmce-orbiter/bin
	cp $(DIANEMO_SRC)/lib/lib{PlutoUtils,SerializeClass,SDL_Helpers,DCECommon}.so $(DESTDIR)/opt/linuxmce-orbiter/lib
	mkdir -p $(DESTDIR)/usr/share/dbus-1/services
	cp linuxmce-orbiter.service $(DESTDIR)/usr/share/dbus-1/services
	mkdir -p $(DESTDIR)/usr/share/applications/hildon
	cp linuxmce-orbiter.desktop $(DESTDIR)/usr/share/applications/hildon
	mkdir -p $(DESTDIR)/usr/share
	cp -r icons $(DESTDIR)/usr/share
	mkdir -p $(DESTDIR)/usr/share/LinuxMCEOrbiter
	cp LinuxMCELogo.png $(DESTDIR)/usr/share/LinuxMCEOrbiter
	find $(DESTDIR) -type d -name .svn -exec rm -rf '{}' ';' -prune

clean:
	rm -f orbiter-launcher .build-timestamp .libs-timestamp

.PHONY: build libs
