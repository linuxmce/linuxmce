all: cegcc libs bins
install: all
	mkdir -p TabletOrbiter
	cp -a OrbiterCE/OrbiterCE.exe OrbiterCE/WaitCursor OrbiterCE/*.png OrbiterCE/OrbiterCE.ini TabletOrbiter
	cp zlib-1.2.5/zlib1.dll libpng-1.4.3/libpng14.dll jpeg-6b-src/src/jpeg-6b-src/jpeg62.dll TabletOrbiter
	cp /opt/mingw32ce/bin/libgcc_s_sjlj-1.dll /opt/mingw32ce/arm-mingw32ce/bin/mingwm10.dll TabletOrbiter

cegcc:
	make -f CEGCC.mk

export PATH:=/opt/mingw32ce/bin:${PATH}

libs: zlib-1.2.5/zlib1.dll libpng-1.4.3/libpng14.dll jpeg-6b-src/src/jpeg-6b-src/jpeg62.dll
bins: OrbiterCE/OrbiterCE.exe

OrbiterCE/OrbiterCE.exe:
	make -C OrbiterCE

zlib-1.2.5: zlib-1.2.5.tar.bz2
	tar -xjf zlib-1.2.5.tar.bz2 || (rm -rf $@ || /bin/false)
	touch $@
zlib-1.2.5/zlib1.dll: zlib-1.2.5
	cd zlib-1.2.5 && make -f ../zlib.mingw32ce.mk

libpng-1.4.3: libpng-1.4.3.tar.bz2
	tar -xjf libpng-1.4.3.tar.bz2 || (rm -rf $@ || /bin/false)
	touch $@
libpng-1.4.3/libpng14.dll: libpng-1.4.3
	cd libpng-1.4.3 && make -f ../libpng.mingw32ce.mk ZLIBINC=../zlib-1.2.5 ZLIBLIB=../zlib-1.2.5

jpeg-6b-src: jpeg-6b-src.tar.bz2
	tar -xjf jpeg-6b-src.tar.bz2 || (rm -rf $@ || /bin/false)
	touch $@
jpeg-6b-src/src/jpeg-6b-src/jpeg62.dll: jpeg-6b-src
	cd jpeg-6b-src/src && make -f ../release/Makefile compilation

jpeg-6b-src.tar.bz2:
	wget 'http://downloads.sourceforge.net/project/cegcc/ported%20packages/libjpeg-6b/jpeg-6b-src.tar.bz2?use_mirror=netcologne&ts=1279828424' \
		|| (rm -f $@ && /bin/false)
zlib-1.2.5.tar.bz2:
	wget 'http://downloads.sourceforge.net/project/libpng/zlib/1.2.5/zlib-1.2.5.tar.bz2?use_mirror=ignum&ts=1279830004' \
		|| (rm -f $@ && /bin/false)
libpng-1.4.3.tar.bz2:
	wget 'http://downloads.sourceforge.net/project/libpng/01-libpng-master/1.4.3/libpng-1.4.3.tar.bz2?use_mirror=mesh&ts=1279829417' \
		|| (rm -f $@ && /bin/false)

clean dist-clean:
	rm -rf zlib-1.2.5* libpng-1.4.3* jpeg-6b-src/* cegcc
	make -C OrbiterCE clean

.PHONY: all cegcc libs bins install
