#!/bin/bash -xe

MAINFOLDER=$(pwd)
ADDFOLDER="$MAINFOLDER/add"
TGZFOLDER="$MAINFOLDER/tgz"
SRCFOLDER="$MAINFOLDER/src"
PKGFOLDER="$MAINFOLDER/pkg/root"

PrepareForWork()
{
	rm -rf "$PKGFOLDER" "$SRCFOLDER"

	rm -rf asterisk-pluto*
	mkdir -p "$PKGFOLDER"{/etc/asterisk,/usr{/lib/asterisk/modules,/sbin,/pluto/bin,/share/asterisk/{agi-bin,sounds}},/../debian,/var/lib/asterisk/sounds/pluto}
	mkdir -p "$SRCFOLDER"

	#########
	cd "$SRCFOLDER"

	local I
	for I in "$TGZFOLDER"/*.tar.gz; do
		echo "Unpacking $I"
		tar -xzf "$I"
	done 
}

Build_AppConference()
{
	cd "$MAINFOLDER/appconference/"
	make clean
	make ASTERISK_INCLUDE_DIR=../backported/asterisk-1.4.10/include INSTALL_PREFIX="$PKGFOLDER" || exit
	make ASTERISK_INCLUDE_DIR=../backported/asterisk-1.4.10/include INSTALL_PREFIX="$PKGFOLDER" install || exit
}

Build_IaxyProv()
{
	cd "$SRCFOLDER/iaxyprov/"
	make clean
	make all
	cp iaxyprov "$PKGFOLDER/usr/sbin/"
	cp iaxydebug "$PKGFOLDER/usr/sbin/"
	strip "$PKGFOLDER/usr/sbin/iaxyprov"
	strip "$PKGFOLDER/usr/sbin/iaxydebug"
}

TheRest()
{
	echo "Copy files for build deb packages...."
	#copy files needed for dpkg-buildpackage
	cp ${ADDFOLDER}/debian-ubuntu/* ${PKGFOLDER}/../debian
	cp ${ADDFOLDER}/Makefile ${PKGFOLDER}/../

	cp -R ${ADDFOLDER}/pluto-sounds/*  ${PKGFOLDER}/var/lib/asterisk/sounds/pluto/
	ln -nsf /var/lib/asterisk/sounds/pluto ${PKGFOLDER}/usr/share/asterisk/sounds/

	#copy AGI scripts
	cp -R ${ADDFOLDER}/linuxmce-validate-user-pin.agi ${PKGFOLDER}/usr/share/asterisk/agi-bin/
	cp -R ${ADDFOLDER}/linuxmce-phonebook-lookup.agi ${PKGFOLDER}/usr/share/asterisk/agi-bin/
	cp -R ${ADDFOLDER}/pluto-sos.agi ${PKGFOLDER}/usr/share/asterisk/agi-bin/
	cp -R ${ADDFOLDER}/pluto-sos-gen.pl ${PKGFOLDER}/usr/pluto/bin/
	cp -R ${ADDFOLDER}/pluto-sos-neighbor.pl ${PKGFOLDER}/usr/pluto/bin/
	cp -R ${ADDFOLDER}/pluto-sos-speak.pl ${PKGFOLDER}/usr/pluto/bin/
	cp -R ${ADDFOLDER}/pluto-gethousemode.agi ${PKGFOLDER}/usr/share/asterisk/agi-bin/
	cp -R ${ADDFOLDER}/pluto-getusermode.agi ${PKGFOLDER}/usr/share/asterisk/agi-bin/
	cp -R ${ADDFOLDER}/pluto-callersforme.agi ${PKGFOLDER}/usr/share/asterisk/agi-bin/

	cd ${PKGFOLDER}/../

	#strip binaries 
	for I in `du -a | grep -E '\.((so)|(a)|(so\..+))$' | cut -f 2`
	do
		strip $I
	done
}

CreatePackage()
{
	dpkg-buildpackage -b -rfakeroot -us -uc
}

PrepareForWork
#Build_AppConference
Build_IaxyProv
TheRest
CreatePackage
