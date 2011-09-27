export PATH:=/opt/mingw32ce/bin:/opt/x86mingw32ce/bin:${PATH}
export SHELL:=/bin/bash

default:
	@echo "Available targets:"
	@echo "    all-arm"
	@echo "    all-x86"
	
all-arm: cegcc-arm
	make CROSS=arm-mingw32ce- DESTDIR=TouchOrbiter/arm libs bins install-files
	cp /opt/mingw32ce/bin/libgcc_s_sjlj-1.dll /opt/mingw32ce/arm-mingw32ce/bin/mingwm10.dll TouchOrbiter/arm
all-x86: cegcc-x86
	make CROSS=i386-mingw32ce- DESTDIR=TouchOrbiter/x86 libs bins install-files
	cp /opt/x86mingw32ce/bin/libgcc_s_sjlj-1.dll /opt/x86mingw32ce/i386-mingw32ce/bin/mingwm10.dll TouchOrbiter/x86

install-files:
	if [[ -z "$(DESTDIR)" ]]; then \
		false; \
	fi
	mkdir -p "$(DESTDIR)"
	cp -af OrbiterCE/OrbiterCE.exe OrbiterCE/WaitCursor OrbiterCE/*.png OrbiterCE/OrbiterCE.ini "$(DESTDIR)"
	cp zlib-1.2.5/zlib1.dll libpng-1.4.8/libpng14.dll jpeg-6b-src/src/jpeg-6b-src/jpeg62.dll "$(DESTDIR)"
	find "$(DESTDIR)" -type d -name .svn -exec rm -rf '{}' ';' -prune

cegcc-arm:
	make -f CEGCC.mk cegcc/build/mingw32ce/.compiled

cegcc-x86:
	make -f CEGCC.mk cegcc/build/x86mingw32ce/.compiled

libs: zlib-1.2.5/zlib1.dll libpng-1.4.8/libpng14.dll jpeg-6b-src/src/jpeg-6b-src/jpeg62.dll
bins: OrbiterCE/OrbiterCE.exe

OrbiterCE/OrbiterCE.exe:
	make -C OrbiterCE clean OrbiterCE.exe CROSS=$(CROSS)

zlib-1.2.5: zlib-1.2.5.tar.bz2
	tar -xjf zlib-1.2.5.tar.bz2 || (rm -rf $@ || /bin/false)
	touch $@
zlib-1.2.5/zlib1.dll: zlib-1.2.5
	cd zlib-1.2.5 && make -f ../zlib.mingw32ce.mk

libpng-1.4.8: libpng-1.4.8.tar.bz2
	tar -xjf libpng-1.4.8.tar.bz2 || (rm -rf $@ || /bin/false)
	touch $@
libpng-1.4.8/libpng14.dll: libpng-1.4.8
	cd libpng-1.4.8 && make -f ../libpng.mingw32ce.mk ZLIBINC=../zlib-1.2.5 ZLIBLIB=../zlib-1.2.5

jpeg-6b-src: jpeg-6b-src.tar.bz2
	tar -xjf jpeg-6b-src.tar.bz2 || (rm -rf $@ || /bin/false)
	cp libjpeg.mingw32ce.mk jpeg-6b-src/src/jpeg-6b-src/Makefile
	touch $@
jpeg-6b-src/src/jpeg-6b-src/jpeg62.dll: jpeg-6b-src
	cd jpeg-6b-src/src && make -f ../release/Makefile compilation

jpeg-6b-src.tar.bz2:
	wget -O $@ 'http://downloads.sourceforge.net/project/cegcc/ported%20packages/libjpeg-6b/jpeg-6b-src.tar.bz2?use_mirror=netcologne&ts=1279828424' \
		|| (rm -f $@ && /bin/false)
zlib-1.2.5.tar.bz2:
	wget -O $@ 'http://downloads.sourceforge.net/project/libpng/zlib/1.2.5/zlib-1.2.5.tar.bz2?use_mirror=ignum&ts=1279830004' \
		|| (rm -f $@ && /bin/false)
libpng-1.4.8.tar.bz2:
	wget -O $@ 'http://downloads.sourceforge.net/project/libpng/libpng14/1.4.8/libpng-1.4.8.tar.bz2?ts=1317140217&use_mirror=ignum' \
		|| (rm -f $@ && /bin/false)

clean:
	rm -rf zlib-1.2.5 libpng-1.4.8 jpeg-6b-src
	make -C OrbiterCE clean
distclean: clean
	rm -rf zlib-1.2.5* libpng-1.4.8* jpeg-6b-src*
	rm -rf cegcc

.PHONY: default cegcc libs bins install clean dist-clean
