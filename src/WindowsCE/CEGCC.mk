all: cegcc/mingw32ce/.compiled

cegcc:
	svn co https://cegcc.svn.sourceforge.net/svnroot/cegcc/trunk/cegcc

cegcc/mingw32ce/.compiled: cegcc
	mkdir -p cegcc/mingw32ce
	@echo "*** I need root access to install CEGCC in /opt/mingw32ce ***"
	cd cegcc/mingw32ce && sudo ../src/scripts/build-mingw32ce.sh
	touch $@

.PHONY: all
