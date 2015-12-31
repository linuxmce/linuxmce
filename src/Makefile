# export CROSS=/stuff/tmp/work/armv4t-angstrom-linux-gnueabi/gcc-cross-4.2.4-r5/staging-pkg/cross/bin/arm-angstrom-linux-gnueabi-
# export CPPFLAGS=-I/stuff/tmp/staging/armv4t-angstrom-linux-gnueabi/usr/include/mysql/
# export CXXFLAGS=-msoft-float -D__GCC_FLOAT_NOT_NEEDED -march=armv4 -mtune=arm920t -Wcast-align
# export CFLAGS=-msoft-float -D__GCC_FLOAT_NOT_NEEDED -march=armv4 -mtune=arm920t -Wcast-align 
# export LDFLAGS=-L/stuff/tmp/staging/armv4t-angstrom-linux-gnueabi/usr/lib/mysql/
# export CC=${CROSS}gcc ${CFLAGS}
#export LD=${CROSS}ld
# export CXX=${CROSS}g++ ${CXXFLAGS}
export LDFLAGS=-L/usr/lib64/mysql

LIBRARIES=RA SerializeClass PlutoUtils DCE pluto_main
APPS=DCERouter MessageSend sqlCVS mysql_wrapper CreateDevice
#DCEDEVS=ZWave
PLUGINS=Datagrid_Plugin Lighting_Plugin Climate_Plugin 
CONFLICTING=

all: libs nolibs

nolibs: plugins apps

apps: required_dirs dcedevs intersecting
	for i in $(APPS); do $(MAKE) -C $$i || exit $$?; done

dcedevs: required_dirs
	for i in $(DCEDEVS); do $(MAKE) -C $$i bin || exit $$?; done

libs: required_dirs
	for i in $(LIBRARIES); do $(MAKE) -C $$i || exit $$?; done

plugins: required_dirs
	for i in $(PLUGINS); do $(MAKE) -C $$i so || exit $$?; done

intersecting: required_dirs
	for i in $(CONFLICTING); do $(MAKE) -C $$i clean all || exit $$?; done

required_dirs:
	mkdir -p bin lib 2>/dev/null

# you have to request yourself; there's no target depending on it
packages: all
	@echo "** Expect lots of editing to do :) Press enter to start"
	read
	for i in $(APPS) $(DCEDEVS) $(PLUGINS) $(CONFLICTING); do $(MAKE) -C $$i package; done

distclean: clean

clean:
	@#for i in $(STANDALONE) $(PLUGINS) $(CONFLICTING); do $(MAKE) -C $$i clean || exit $$?; done
	for i in '*.d' '*.d.*' '*.o'; do find -type f -name "$$i" -exec rm '{}' ';' ; done

.PHONY: clean all


install: 
	for i in $(APPS) $(DCEDEVS) $(PLUGINS) $(CONFLICTING); do $(MAKE) -C $$i install; done
	mkdir -p $(DESTDIR)/home/public $(DESTDIR)/usr/pluto/bin $(DESTDIR)/usr/pluto/lib $(DESTDIR)/usr/include
#	cp BootScripts/bookmarks.html $(DESTDIR)/home/public || :
	for i in $(PLUGINS); do cp $$i/$$i.so $(DESTDIR)/usr/pluto/bin; done
	for i in $(APPS); do cp $$i/$$i $(DESTDIR)/usr/pluto/bin; done
	for i in $(LIBRARIES); do cp lib/lib$$i* $(DESTDIR)/usr/pluto/lib; done
	for i in $(LIBRARIES); do mkdir -p $(DESTDIR)/usr/include/$$i ; cp $$i/*.h $(DESTDIR)/usr/include/$$i ; done
