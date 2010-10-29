all: cegcc/build/mingw32ce/.compiled cegcc/build/x86mingw32ce/.compiled

cegcc:
	svn co https://cegcc.svn.sourceforge.net/svnroot/cegcc/trunk/cegcc

cegcc/build/mingw32ce/.compiled: cegcc
	mkdir -p cegcc/build/mingw32ce
	@echo "*** I need root access to install CEGCC in /opt/mingw32ce ***"
	cd cegcc/build/mingw32ce && sudo ../../src/scripts/build-mingw32ce.sh
	touch $@

cegcc/build/x86mingw32ce/.compiled: cegcc
	mkdir -p cegcc/build/x86mingw32ce
	@echo "*** I need root access to install CEGCC in /opt/x86mingw32ce ***"
	cd cegcc/build/x86mingw32ce && sudo ../../src/scripts/build-x86.sh
	touch $@

.PHONY: all
